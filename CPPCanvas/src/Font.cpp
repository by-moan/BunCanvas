std::unordered_map<std::string, sk_sp<SkTypeface>> fontRegistry;

struct CStringHash {
    size_t operator()(const char* str) const noexcept {
        // FNV-1a hash
        size_t hash = 14695981039346656037ull;
        while (*str) {
            hash ^= static_cast<size_t>(*str++);
            hash *= 1099511628211ull;
        }
        return hash;
    }
};

struct CStringEq {
    bool operator()(const char* a, const char* b) const noexcept {
        return std::strcmp(a, b) == 0;
    }
};

class CanvasFontCache {
public:
    struct CachedFont {
        SkFont font;
        bool small_caps;
    };

private:
    // Primary cache: const char* (owned by JS/Caller) -> CachedFont
    // Note: Assumes cssString lifetime persists or we copy it
    std::unordered_map<const char*, CachedFont, CStringHash, CStringEq> cache_;
    
    // Typeface cache: family name -> typeface (CRITICAL: avoids SkFontMgr lookups)
    std::unordered_map<std::string, sk_sp<SkTypeface>> typeface_cache_;
    
    // Reusable buffers to avoid allocations
    std::vector<std::string> family_buffer_;
    std::string temp_family_;
    
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

public:
    CanvasFontCache(){
        // Pre-allocate to avoid rehashing
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
        
        // Parse (construct std::string only for storage, not lookup)
        SkFontStyle style;
        float size_px = 0.0f;
        bool small_caps = false;
        
        // Clear reusable buffer
        family_buffer_.clear();
        
        if (!parseFast(cssString, style, size_px, family_buffer_, small_caps)) {
            return false;
        }
        
        // Resolve typeface (cached by family name)
        sk_sp<SkTypeface> typeface = resolveTypefaceCached(family_buffer_, style);
        
        // Create font
        SkFont font(typeface, size_px);
        
        // Store in cache (copy string for storage)
        std::string key(cssString);
        auto& entry = cache_[key.c_str()]; // Store pointer to string data
        entry = {font, small_caps};
        
        target = font;
        return true;
    }

private:
    // Ultra-fast parser for common patterns: "bold 16px Arial" or "700 16px Arial"
    // Falls back to full parser if pattern doesn't match
    bool parseFast(const char* str, SkFontStyle& style, float& size, 
                   std::vector<std::string>& families, bool& small_caps) {
        small_caps = false;
        const char* p = str;
        
        // Skip leading whitespace
        while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        if (!*p) return false;
        
        // Try to parse: [weight] [size]px [family]
        // Weight: 100-900 or bold/normal
        int weight = SkFontStyle::kNormal_Weight;
        
        // Check for number (weight)
        if (std::isdigit(static_cast<unsigned char>(*p))) {
            weight = 0;
            while (*p && std::isdigit(static_cast<unsigned char>(*p))) {
                weight = weight * 10 + (*p - '0');
                p++;
            }
            while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        } else if (std::isalpha(static_cast<unsigned char>(*p))) {
            // Check for "bold" or "normal"
            if (strncasecmp(p, "bold", 4) == 0 && 
                (p[4] == '\0' || std::isspace(static_cast<unsigned char>(p[4])))) {
                weight = SkFontStyle::kBold_Weight;
                p += 4;
                while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
            } else if (strncasecmp(p, "normal", 6) == 0 && 
                      (p[6] == '\0' || std::isspace(static_cast<unsigned char>(p[6])))) {
                weight = SkFontStyle::kNormal_Weight;
                p += 6;
                while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
            }
        }
        
        // Parse size: digits followed by px/pt/etc
        if (!std::isdigit(static_cast<unsigned char>(*p))) {
            // Fall back to full CSS parser
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
            // Unknown unit or no unit, fall back
            return parseFullCSS(str, style, size, families, small_caps);
        }
        
        // Skip whitespace
        while (*p && std::isspace(static_cast<unsigned char>(*p))) p++;
        
        // Rest is family (single family for fast path)
        if (*p) {
            families.emplace_back(p); // Copies remaining string
        } else {
            return false;
        }
        
        style = SkFontStyle(weight, SkFontStyle::kNormal_Width, SkFontStyle::kUpright_Slant);
        return true;
    }
    bool parseFullCSS(const char* str, SkFontStyle& style, float& size,
                     std::vector<std::string>& families, bool& small_caps) {
        return false;
    }
    
    // CRITICAL OPTIMIZATION: Cache typefaces by family name
    sk_sp<SkTypeface> resolveTypefaceCached(const std::vector<std::string>& families,
                                            const SkFontStyle& style) {
        for (const auto& fam : families) {
            // Check typeface cache first (O(1) string hash)
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
            
            // Expensive: Query system
            sk_sp<SkTypeface> tf = fontMgr->matchFamilyStyle(fam.c_str(), style);
            if (tf) {
                typeface_cache_[fam] = tf; // Cache it!
                return tf;
            }
        }
        
        return fontMgr->legacyMakeTypeface(nullptr, style);
    }
};

extern "C" {
    // Register a custom font for fast lookup
    void* font_load(const char* path){
        return fontMgr->makeFromFile(path).release();
    }

    void font_add_to_registry(const char* family_name, void* typeface) {
        auto _ptr = static_cast<SkTypeface*>(typeface);
        if (!_ptr) return;
        auto typ = sk_sp<SkTypeface>(_ptr);
        fontRegistry[family_name] = std::move(typ);
    }
}