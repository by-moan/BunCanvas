
namespace font_parser {

struct Tokenizer {
    explicit Tokenizer(const std::string& s) : str(s), pos(0) {}

    std::string next() {
        skipSpaces();
        if (pos >= str.size()) return {};

        if (str[pos] == '"' || str[pos] == '\'') {
            char quote = str[pos++];
            size_t start = pos;
            while (pos < str.size() && str[pos] != quote) ++pos;
            std::string tok = str.substr(start, pos - start);
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

private:
    void skipSpaces() {
        while (pos < str.size() && str[pos] == ' ') ++pos;
    }

    const std::string& str;
    size_t pos;
};
    
    inline bool iequals(const std::string& a, const std::string& b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i)
            if (std::tolower(static_cast<unsigned char>(a[i])) !=
                std::tolower(static_cast<unsigned char>(b[i])))
                return false;
        return true;
    }
    
    inline const std::vector<std::string> STYLE_KW = {"normal", "italic", "oblique"};
    inline const std::vector<std::string> VARIANT_KW = {"normal", "small-caps"};
    inline const std::vector<std::string> WEIGHT_KW = {"normal", "bold", "bolder", "lighter"};
    inline const std::vector<std::string> STRETCH_KW = {
        "normal", "ultra-condensed", "extra-condensed", "condensed",
        "semi-condensed", "semi-expanded", "expanded", "extra-expanded",
        "ultra-expanded"
    };
    inline const std::vector<std::string> SIZE_KW = {
        "xx-small", "x-small", "small", "medium",
        "large", "x-large", "xx-large", "xxx-large"
    };
    
    inline SkFontStyle::Weight weightKeywordToEnum(const std::string& kw) {
        static const std::unordered_map<std::string, SkFontStyle::Weight> map = {
            {"thin", SkFontStyle::Weight::kThin_Weight}, {"extralight", SkFontStyle::Weight::kExtraLight_Weight}, {"light", SkFontStyle::Weight::kLight_Weight}, {"normal", SkFontStyle::Weight::kNormal_Weight},
            {"medium", SkFontStyle::Weight::kMedium_Weight}, {"semibold", SkFontStyle::Weight::kSemiBold_Weight}, {"bold", SkFontStyle::Weight::kBold_Weight},
            {"extrabold", SkFontStyle::Weight::kExtraBold_Weight}, {"black", SkFontStyle::Weight::kBlack_Weight},
        };
        auto it = map.find(kw);
        return it != map.end() ? it->second : SkFontStyle::kNormal_Weight;
    }
    inline SkFontStyle::Width stretchKeywordToEnum(const std::string& kw) {
        static const std::unordered_map<std::string, SkFontStyle::Width> map = {
            {"ultra-condensed", SkFontStyle::kUltraCondensed_Width},
            {"extra-condensed", SkFontStyle::kExtraCondensed_Width},
            {"condensed",       SkFontStyle::kCondensed_Width},
            {"semi-condensed",  SkFontStyle::kSemiCondensed_Width},
            {"normal",          SkFontStyle::kNormal_Width},
            {"semi-expanded",   SkFontStyle::kSemiExpanded_Width},
            {"expanded",        SkFontStyle::kExpanded_Width},
            {"extra-expanded",  SkFontStyle::kExtraExpanded_Width},
            {"ultra-expanded",  SkFontStyle::kUltraExpanded_Width},
        };
        auto it = map.find(kw);
        return it != map.end() ? it->second : SkFontStyle::kNormal_Width;
    }
    inline SkFontStyle::Slant styleKeywordToEnum(const std::string& kw) {
        if (iequals(kw, "italic")) return SkFontStyle::kItalic_Slant;
        if (iequals(kw, "oblique")) return SkFontStyle::kOblique_Slant;
        return SkFontStyle::kUpright_Slant;
    }
    
    struct ParsedFont {
        SkFontStyle::Weight weight = SkFontStyle::kNormal_Weight;
        SkFontStyle::Width  width  = SkFontStyle::kNormal_Width;
        SkFontStyle::Slant  slant  = SkFontStyle::kUpright_Slant;
        float               sizePx = 16.0f;
        std::string         family;
    };
    
    float cssSizeToPixels(const std::string& token) {
        static const std::regex re(R"(^(-?[\d.]+)\s*(px|pt|pc|in|cm|mm|q|em|rem|ex|ch|%|vw|vh|vmin|vmax)?$)", std::regex_constants::icase);
        std::smatch m;
        if (!std::regex_match(token, m, re))
            throw std::runtime_error("toPixels: cannot parse \"" + token + "\"");
    
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
            return val;   // placeholder
    
        throw std::runtime_error("toPixels: unknown unit \"" + unit + "\"");
    }
    
    ParsedFont parseFontShorthand(const std::string& css) {
    Tokenizer tok(css);
    ParsedFont out;

    while (tok.hasMore()) {
        std::string t = tok.next();
        std::string low = t;
        std::transform(low.begin(), low.end(), low.begin(),
                      [](unsigned char c){ return std::tolower(c); });

        // style
        if (std::find(STYLE_KW.begin(), STYLE_KW.end(), low) != STYLE_KW.end()) {
            if (low == "italic" || low == "oblique")
                out.slant = styleKeywordToEnum(low);
            if (low == "oblique" && tok.hasMore()) {
                std::string nxt = tok.next(); // consume optional style modifier
            }
            continue;
        }

        if (std::find(VARIANT_KW.begin(), VARIANT_KW.end(), low) != VARIANT_KW.end()) {
            continue;
        }

        if (std::find(WEIGHT_KW.begin(), WEIGHT_KW.end(), low) != WEIGHT_KW.end()) {
            out.weight = weightKeywordToEnum(low);
            continue;
        }

        if (std::regex_match(low, std::regex("^(100|200|300|400|500|600|700|800|900)$"))) {
            out.weight = static_cast<SkFontStyle::Weight>(std::stoi(low));
            continue;
        }

        if (std::find(STRETCH_KW.begin(), STRETCH_KW.end(), low) != STRETCH_KW.end()) {
            out.width = stretchKeywordToEnum(low);
            continue;
        }

        // If we reach here, 't' is assumed to be the font-size
        std::string sizeTok = t;
        std::string family; // Declare family here so we can populate it early

        auto slashPos = sizeTok.find('/');
        if (slashPos != std::string::npos) {
            sizeTok = sizeTok.substr(0, slashPos);
        } else if (tok.hasMore()) {
            std::string nxt = tok.next(); 
            if (nxt == "/") {
                // It was a slash; consume the line-height value that follows
                if (tok.hasMore()) tok.next();
            } else {
                // It wasn't a slash; it's the first part of the family name
                family = nxt;
            }
        }

        out.sizePx = cssSizeToPixels(sizeTok);

        // Collect remaining tokens as family
        while (tok.hasMore()) {
            std::string part = tok.next();
            if (!family.empty()) family += ' ';
            family += part;
        }
        out.family = family;
        return out;
    }

    throw std::runtime_error("parseFont: no font-size found in " + css);
}
    std::string cleanFamilyList(const std::string& raw) {
        std::string out;
        bool inQuote = false;
        char quoteChar = 0;
        for (size_t i = 0; i < raw.size(); ++i) {
            char c = raw[i];
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
        std::string collapsed;
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
        size_t start = 0;
        while (start < collapsed.size() && collapsed[start] == ' ') ++start;
        size_t end = collapsed.size();
        while (end > start && collapsed[end - 1] == ' ') --end;
        return collapsed.substr(start, end - start);
    }
}

SkFontStyle parseCssToSkFontStyle(const std::string& css) {
    using namespace font_parser;
    ParsedFont pf = parseFontShorthand(css);
    return SkFontStyle(pf.weight, pf.width, pf.slant);
}

std::string parseCssFamilyList(const std::string& css) {
    using namespace font_parser;
    ParsedFont pf = parseFontShorthand(css);
    return cleanFamilyList(pf.family);
}

float parseCssFontSize(const std::string& css) {
    using namespace font_parser;
    ParsedFont pf = parseFontShorthand(css);
    return pf.sizePx;
}