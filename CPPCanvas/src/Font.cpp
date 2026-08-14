std::unordered_map<std::string, sk_sp<SkTypeface>> fontRegistry;

// Transparent hash for fast lookup without string construction
struct StringHash {
    using is_transparent = void;
    size_t operator()(std::string_view sv) const noexcept {
        // FNV-1a hash
        size_t hash = 14695981039346656037ull;
        for (char c : sv) {
            hash ^= static_cast<size_t>(c);
            hash *= 1099511628211ull;
        }
        return hash;
    }
};

struct StringEq {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept {
        return a == b;
    }
};

class CanvasFontCache {
public:
    struct CachedFont {
        SkFont font;
        bool small_caps;
    };

private:
    // Primary cache with transparent hash for fast lookup
    std::unordered_map<std::string, CachedFont, StringHash, StringEq> cache_;
    
    // Typeface cache: family name -> typeface
    std::unordered_map<std::string, sk_sp<SkTypeface>> typeface_cache_;
    
    // Reusable buffers to avoid allocations
    std::vector<std::string> family_buffer_;
    
    // Parser state
    struct ParsedFont {
        SkFontStyle::Weight weight = SkFontStyle::kNormal_Weight;
        SkFontStyle::Width  width  = SkFontStyle::kNormal_Width;
        SkFontStyle::Slant  slant  = SkFontStyle::kUpright_Slant;
        float               sizePx = 16.0f;
        std::string         family;
        bool                small_caps = false;
    };
    
    // String view tokenizer - zero copy
    struct Tokenizer {
        std::string_view str;
        size_t pos = 0;
        
        explicit Tokenizer(std::string_view s) : str(s) {}
        
        void skipSpaces() {
            while (pos < str.size() && str[pos] == ' ') ++pos;
        }
        
        std::string_view next() {
            skipSpaces();
            if (pos >= str.size()) return {};
            
            if (str[pos] == '"' || str[pos] == '\'') {
                char quote = str[pos++];
                size_t start = pos;
                while (pos < str.size() && str[pos] != quote) ++pos;
                std::string_view tok = str.substr(start, pos - start);
                if (pos < str.size()) ++pos;
                return tok;
            }
            
            size_t start = pos;
            while (pos < str.size() && str[pos] != ' ') ++pos;
            return str.substr(start, pos - start);
        }
        
        bool hasMore() const {
            size_t p = pos;
            while (p < str.size() && str[p] == ' ') ++p;
            return p < str.size();
        }
        
        std::string_view peek() const {
            size_t p = pos;
            while (p < str.size() && str[p] == ' ') ++p;
            if (p >= str.size()) return {};
            
            if (str[p] == '"' || str[p] == '\'') {
                char quote = str[p++];
                size_t start = p;
                while (p < str.size() && str[p] != quote) ++p;
                return str.substr(start, p - start);
            }
            
            size_t start = p;
            while (p < str.size() && str[p] != ' ') ++p;
            return str.substr(start, p - start);
        }
    };
    
    static std::string_view trim(std::string_view sv) {
        while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) sv.remove_prefix(1);
        while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) sv.remove_suffix(1);
        return sv;
    }
    
    static bool iequals(std::string_view a, std::string_view b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(a[i])) != 
                std::tolower(static_cast<unsigned char>(b[i]))) return false;
        }
        return true;
    }
    
    // Fast keyword matching without allocation
    static bool is_keyword(std::string_view str, std::string_view kw) {
        if (str.size() != kw.size()) return false;
        for (size_t i = 0; i < kw.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(str[i])) != kw[i]) return false;
        }
        return true;
    }
    
    static SkFontStyle::Weight weightKeywordToEnum(std::string_view kw) {
        if (is_keyword(kw, "thin")) return SkFontStyle::kThin_Weight;
        if (is_keyword(kw, "extralight") || is_keyword(kw, "extra-light")) return SkFontStyle::kExtraLight_Weight;
        if (is_keyword(kw, "light")) return SkFontStyle::kLight_Weight;
        if (is_keyword(kw, "normal")) return SkFontStyle::kNormal_Weight;
        if (is_keyword(kw, "medium")) return SkFontStyle::kMedium_Weight;
        if (is_keyword(kw, "semibold") || is_keyword(kw, "semi-bold")) return SkFontStyle::kSemiBold_Weight;
        if (is_keyword(kw, "bold")) return SkFontStyle::kBold_Weight;
        if (is_keyword(kw, "extrabold") || is_keyword(kw, "extra-bold")) return SkFontStyle::kExtraBold_Weight;
        if (is_keyword(kw, "black")) return SkFontStyle::kBlack_Weight;
        if (is_keyword(kw, "bolder")) return SkFontStyle::kBold_Weight; // Approximation
        if (is_keyword(kw, "lighter")) return SkFontStyle::kLight_Weight; // Approximation
        
        // Numeric weight
        if (!kw.empty() && std::isdigit(static_cast<unsigned char>(kw[0]))) {
            int val = 0;
            for (char c : kw) {
                if (!std::isdigit(static_cast<unsigned char>(c))) break;
                val = val * 10 + (c - '0');
            }
            if (val >= 100 && val <= 900) return static_cast<SkFontStyle::Weight>(val);
        }
        return SkFontStyle::kNormal_Weight;
    }
    
    static SkFontStyle::Width stretchKeywordToEnum(std::string_view kw) {
        if (is_keyword(kw, "ultra-condensed")) return SkFontStyle::kUltraCondensed_Width;
        if (is_keyword(kw, "extra-condensed")) return SkFontStyle::kExtraCondensed_Width;
        if (is_keyword(kw, "condensed")) return SkFontStyle::kCondensed_Width;
        if (is_keyword(kw, "semi-condensed")) return SkFontStyle::kSemiCondensed_Width;
        if (is_keyword(kw, "normal")) return SkFontStyle::kNormal_Width;
        if (is_keyword(kw, "semi-expanded")) return SkFontStyle::kSemiExpanded_Width;
        if (is_keyword(kw, "expanded")) return SkFontStyle::kExpanded_Width;
        if (is_keyword(kw, "extra-expanded")) return SkFontStyle::kExtraExpanded_Width;
        if (is_keyword(kw, "ultra-expanded")) return SkFontStyle::kUltraExpanded_Width;
        return SkFontStyle::kNormal_Width;
    }
    
    static SkFontStyle::Slant styleKeywordToEnum(std::string_view kw) {
        if (is_keyword(kw, "italic")) return SkFontStyle::kItalic_Slant;
        if (is_keyword(kw, "oblique")) return SkFontStyle::kOblique_Slant;
        return SkFontStyle::kUpright_Slant;
    }
    
    std::optional<float> cssSizeToPixels(std::string_view token) {
        // Fast path for px (most common)
        if (token.size() > 2 && 
            (token[token.size()-2] == 'p' || token[token.size()-2] == 'P') &&
            (token[token.size()-1] == 'x' || token[token.size()-1] == 'X')) {
            try {
                return std::stof(std::string(token.substr(0, token.size()-2)));
            } catch (...) {
                return std::nullopt;
            }
        }
        
        // Full regex path for other units
        static const std::regex re(R"(^(-?[\d.]+)\s*(px|pt|pc|in|cm|mm|q|em|rem|ex|ch|%|vw|vh|vmin|vmax)?$)", 
                                   std::regex_constants::icase);
        std::smatch m;
        std::string tok_str(token);
        if (!std::regex_match(tok_str, m, re)) return std::nullopt;
        
        float val = std::stof(m[1].str());
        std::string unit = m[2].str();
        std::transform(unit.begin(), unit.end(), unit.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        
        const float rootFontSize = 16.0f;
        const float parentFontSize = 16.0f;
        const float exSize = 8.0f;
        const float chSize = 8.0f;
        
        if (unit.empty() || unit == "px") return val;
        if (unit == "pt") return val * 96.0f / 72.0f;
        if (unit == "pc") return val * 16.0f;
        if (unit == "in") return val * 96.0f;
        if (unit == "cm") return val * 96.0f / 2.54f;
        if (unit == "mm") return val * 96.0f / 25.4f;
        if (unit == "q")  return val * 96.0f / 25.4f / 4.0f;
        if (unit == "em") return val * parentFontSize;
        if (unit == "rem") return val * rootFontSize;
        if (unit == "ex") return val * exSize;
        if (unit == "ch") return val * chSize;
        if (unit == "%")  return val * parentFontSize / 100.0f;
        if (unit == "vw" || unit == "vh" || unit == "vmin" || unit == "vmax")
            return val;
            
        return std::nullopt;
    }
    
    bool parseFontShorthand(std::string_view css, ParsedFont& out) {
        Tokenizer tok(css);
        
        while (tok.hasMore()) {
            std::string_view t = tok.next();
            std::string low;
            low.reserve(t.size());
            for (char c : t) low.push_back(std::tolower(static_cast<unsigned char>(c)));
            
            // Style keywords
            if (low == "normal" || low == "italic" || low == "oblique") {
                if (low == "italic" || low == "oblique")
                    out.slant = styleKeywordToEnum(t);
                if (low == "oblique" && tok.hasMore()) {
                    // Consume optional angle
                    std::string_view nxt = tok.peek();
                    if (!nxt.empty() && (nxt[0] == '-' || std::isdigit(nxt[0]))) {
                        tok.next();
                    }
                }
                continue;
            }
            
            // Variant
            if (low == "normal" || low == "small-caps") {
                if (low == "small-caps") out.small_caps = true;
                continue;
            }
            
            // Weight keywords
            if (low == "normal" || low == "bold" || low == "bolder" || low == "lighter") {
                out.weight = weightKeywordToEnum(low);
                continue;
            }
            
            // Numeric weight
            if (std::regex_match(low, std::regex("^(100|200|300|400|500|600|700|800|900)$"))) {
                out.weight = static_cast<SkFontStyle::Weight>(std::stoi(low));
                continue;
            }
            
            // Stretch
            static const std::string_view stretches[] = {
                "ultra-condensed", "extra-condensed", "condensed", "semi-condensed",
                "semi-expanded", "expanded", "extra-expanded", "ultra-expanded"
            };
            bool is_stretch = false;
            for (auto s : stretches) {
                if (low == s) { is_stretch = true; break; }
            }
            if (is_stretch || low == "normal") {
                out.width = stretchKeywordToEnum(low);
                continue;
            }
            
            // Size keywords
            static const std::string_view sizes[] = {
                "xx-small", "x-small", "small", "medium", "large", "x-large", "xx-large", "xxx-large"
            };
            for (auto s : sizes) {
                if (low == s) {
                    // Convert size keyword to pixels (approximate)
                    if (s == "xx-small") out.sizePx = 9.0f;
                    else if (s == "x-small") out.sizePx = 10.0f;
                    else if (s == "small") out.sizePx = 13.0f;
                    else if (s == "medium") out.sizePx = 16.0f;
                    else if (s == "large") out.sizePx = 18.0f;
                    else if (s == "x-large") out.sizePx = 24.0f;
                    else if (s == "xx-large") out.sizePx = 32.0f;
                    else if (s == "xxx-large") out.sizePx = 48.0f;
                    
                    // Collect family
                    while (tok.hasMore()) {
                        std::string_view part = tok.next();
                        if (!out.family.empty()) out.family += ' ';
                        out.family += part;
                    }
                    out.family = cleanFamilyList(out.family);
                    return true;
                }
            }
            
            // Must be size (with optional line-height)
            std::string_view sizeTok = t;
            
            auto slashPos = sizeTok.find('/');
            if (slashPos != std::string_view::npos) {
                sizeTok = sizeTok.substr(0, slashPos);
            } else if (tok.hasMore()) {
                std::string_view nxt = tok.peek();
                if (nxt == "/") {
                    tok.next(); // consume slash
                    if (tok.hasMore()) tok.next(); // consume line-height
                }
            }
            
            auto pxSize = cssSizeToPixels(sizeTok);
            if (!pxSize) return false;
            out.sizePx = *pxSize;
            
            // Collect remaining tokens as family
            while (tok.hasMore()) {
                std::string_view part = tok.next();
                if (!out.family.empty()) out.family += ' ';
                out.family += part;
            }
            out.family = cleanFamilyList(out.family);
            return true;
        }
        
        return false;
    }
    
    std::string cleanFamilyList(const std::string& raw) {
        std::string out;
        out.reserve(raw.size());
        bool inQuote = false;
        char quoteChar = 0;
        
        for (char c : raw) {
            if (!inQuote && (c == '"' || c == '\'')) {
                inQuote = true;
                quoteChar = c;
                continue;
            }
            if (inQuote && c == quoteChar) {
                inQuote = false;
                quoteChar = 0;
                continue;
            }
            out.push_back(c);
        }
        
        // Collapse spaces
        std::string collapsed;
        collapsed.reserve(out.size());
        bool lastSpace = false;
        for (char c : out) {
            if (c == ' ') {
                if (!lastSpace) collapsed.push_back(' ');
                lastSpace = true;
            } else {
                collapsed.push_back(c);
                lastSpace = false;
            }
        }
        
        // Trim
        size_t start = 0;
        while (start < collapsed.size() && collapsed[start] == ' ') ++start;
        size_t end = collapsed.size();
        while (end > start && collapsed[end - 1] == ' ') --end;
        return collapsed.substr(start, end - start);
    }

public:
    CanvasFontCache(){
        cache_.reserve(2048);
        typeface_cache_.reserve(256);
        family_buffer_.reserve(8);
    }
    
    void registerFont(const std::string& family_name, sk_sp<SkTypeface> typeface) {
        fontRegistry[family_name] = std::move(typeface);
    }
    
    bool setFont(const char* cssString, SkFont& target) {
        // Fast path: direct hash lookup without string construction
        auto it = cache_.find(cssString);
        if (it != cache_.end()) {
            target = it->second.font;
            return true;
        }
        
        // Parse
        SkFontStyle style;
        float size_px = 0.0f;
        bool small_caps = false;
        
        family_buffer_.clear();
        
        if (!parseFast(cssString, style, size_px, family_buffer_, small_caps)) {
            return false;
        }
        
        // Resolve typeface
        sk_sp<SkTypeface> typeface = resolveTypefaceCached(family_buffer_, style);
        
        // Create font
        SkFont font(typeface, size_px);
        
        // Store in cache
        cache_.emplace(cssString, CachedFont{font, small_caps});
        
        target = font;
        return true;
    }

private:
    bool parseFast(const char* str, SkFontStyle& style, float& size, 
                   std::vector<std::string>& families, bool& small_caps) {
        small_caps = false;
        const char* p = str;
        
        // Skip leading whitespace
        while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        if (!*p) return false;
        
        int weight = SkFontStyle::kNormal_Weight;
        SkFontStyle::Width width = SkFontStyle::kNormal_Width;
        SkFontStyle::Slant slant = SkFontStyle::kUpright_Slant;
        
        // Parse style/weight keywords
        while (true) {
            if (strncasecmp(p, "bold", 4) == 0 && 
                (p[4] == '\0' || std::isspace(static_cast<unsigned char>(p[4])))) {
                weight = SkFontStyle::kBold_Weight;
                p += 4;
            } else if (strncasecmp(p, "normal", 6) == 0 && 
                      (p[6] == '\0' || std::isspace(static_cast<unsigned char>(p[6])))) {
                p += 6;
            } else if (strncasecmp(p, "italic", 6) == 0 && 
                      (p[6] == '\0' || std::isspace(static_cast<unsigned char>(p[6])))) {
                slant = SkFontStyle::kItalic_Slant;
                p += 6;
            } else if (strncasecmp(p, "oblique", 7) == 0 && 
                      (p[7] == '\0' || std::isspace(static_cast<unsigned char>(p[7])))) {
                slant = SkFontStyle::kOblique_Slant;
                p += 7;
            } else {
                break;
            }
            while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        }
        
        // Check for number weight
        if (std::isdigit(static_cast<unsigned char>(*p))) {
            weight = 0;
            while (*p && std::isdigit(static_cast<unsigned char>(*p))) {
                weight = weight * 10 + (*p - '0');
                p++;
            }
            while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        }
        
        // Parse size
        if (!std::isdigit(static_cast<unsigned char>(*p))) {
            return parseFullCSS(str, style, size, families, small_caps);
        }
        
        float num = 0.0f;
        while (*p && std::isdigit(static_cast<unsigned char>(*p))) {
            num = num * 10 + (*p - '0');
            p++;
        }
        
        if (*p == '.') {
            p++;
            float frac = 0.1f;
            while (*p && std::isdigit(static_cast<unsigned char>(*p))) {
                num += (*p - '0') * frac;
                frac *= 0.1f;
                p++;
            }
        }
        
        // Parse unit
        while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        
        if (strncasecmp(p, "px", 2) == 0) {
            size = num;
            p += 2;
        } else if (strncasecmp(p, "pt", 2) == 0) {
            size = num * 96.0f / 72.0f;
            p += 2;
        } else {
            // Unknown unit or complex format, fall back
            return parseFullCSS(str, style, size, families, small_caps);
        }
        
        while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        
        if (!*p) return false;
        
        families.emplace_back(p);
        style = SkFontStyle(weight, width, slant);
        return true;
    }
    
    bool parseFullCSS(const char* str, SkFontStyle& style, float& size,
                     std::vector<std::string>& families, bool& small_caps) {
        ParsedFont pf;
        if (!parseFontShorthand(str, pf)) {
            return false;
        }
        
        style = SkFontStyle(pf.weight, pf.width, pf.slant);
        size = pf.sizePx;
        small_caps = pf.small_caps;
        
        // Split family by comma
        families.clear();
        std::string_view fam(pf.family);
        size_t start = 0;
        
        for (size_t i = 0; i <= fam.size(); ++i) {
            if (i == fam.size() || fam[i] == ',') {
                std::string_view part = trim(fam.substr(start, i - start));
                if (!part.empty()) {
                    // Remove quotes
                    std::string cleaned;
                    cleaned.reserve(part.size());
                    for (char c : part) {
                        if (c != '"' && c != '\'') cleaned.push_back(c);
                    }
                    std::string_view final_part = trim(std::string_view(cleaned));
                    if (!final_part.empty()) {
                        families.emplace_back(final_part);
                    }
                }
                start = i + 1;
            }
        }
        
        return !families.empty();
    }
    
    sk_sp<SkTypeface> resolveTypefaceCached(const std::vector<std::string>& families,
                                            const SkFontStyle& style) {
        for (const auto& fam : families) {
            // Check typeface cache
            auto it = typeface_cache_.find(fam);
            if (it != typeface_cache_.end()) {
                return it->second;
            }
            
            // Check custom registry
            auto reg_it = fontRegistry.find(fam);
            if (reg_it != fontRegistry.end()) {
                typeface_cache_[fam] = reg_it->second;
                return reg_it->second;
            }
            
            // Query system
            sk_sp<SkTypeface> tf = fontMgr->matchFamilyStyle(fam.c_str(), style);
            if (tf) {
                typeface_cache_[fam] = tf;
                return tf;
            }
        }
        
        return fontMgr->legacyMakeTypeface(nullptr, style);
    }
};

extern "C" {
    WINDOWS_EXPORT void* font_load(const char* path){
        return fontMgr->makeFromFile(path).release();
    }

    WINDOWS_EXPORT void font_add_to_registry(const char* family_name, void* typeface) {
        auto _ptr = static_cast<SkTypeface*>(typeface);
        if (!_ptr) return;
        auto typ = sk_sp<SkTypeface>(_ptr);
        fontRegistry[family_name] = std::move(typ);
    }
}