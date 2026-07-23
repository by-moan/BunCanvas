int parseCharValue(const char*& c, bool& isValid){
    int n = 0;
    bool v = false;
    while (*c >= '0' && *c <= '9') {
        v = true;
        n = n * 10 + (*c - '0');
        ++c;
    }
    isValid = v;
    return std::clamp(n, 0, 255);
}

//Skips only a single comma and any number of spaces
void skipAllowed(const char*& c, int& commaCount){
    while (*c == ' ' || *c == ',') {
        if (*c == ',') commaCount++;
        ++c;
    };
}

std::optional<SkColor4f> parseCssColor(const char* c){
    if (!c) return std::nullopt;
    auto it = keywordColors.find(c);
    if (it != keywordColors.end()) return it->second;
    auto len = std::strlen(c);
    //Comparing length
    if (len > 1 && c[0] == '#') {
        //Check if valid before computing
        for (size_t i = 1 ; i < len; i++) {
            if (!(
                (c[i] >= '0' && c[i] <= '9') ||
                (c[i] >= 'a' && c[i] <= 'f') ||
                (c[i] >= 'A' && c[i] <= 'F')
            )) return std::nullopt;
        }
        if (len == 4) {
            unsigned int rgb = std::strtoul(c + 1, nullptr, 16);
            uint8_t r = (rgb >> 8) & 0xF;
            uint8_t g = (rgb >> 4) & 0xF;
            uint8_t b = rgb & 0xF;
            r = (r << 4) | r;
            g = (g << 4) | g;
            b = (b << 4) | b;
            return SkColor4f{
                r / 255.f,
                g / 255.f,
                b / 255.f,
                1.0f
            };
        }
        if (len == 5) {
            unsigned int rgba = std::strtoul(c + 1, nullptr, 16);
            uint8_t r = (rgba >> 12) & 0xF;
            uint8_t g = (rgba >> 8) & 0xF;
            uint8_t b = (rgba >> 4) & 0xF;
            uint8_t a = rgba & 0xF;
            r = (r << 4) | r;
            g = (g << 4) | g;
            b = (b << 4) | b;
            a = (a << 4) | a;
            return SkColor4f{
                r / 255.f,
                g / 255.f,
                b / 255.f,
                a / 255.f
            };
        }
        if (len == 7) {
            unsigned int rgb = std::strtoul(c+1, nullptr, 16);
            return SkColor4f{
                ((rgb >> 16) & 0xFF) / 255.f,
                ((rgb >> 8) & 0xFF) / 255.f,
                (rgb & 0xFF) / 255.f,
                1.0f
            };
        }
        if (len == 9) {
            unsigned int rgb = std::strtoul(c+1, nullptr, 16);
            return SkColor4f{
                ((rgb >> 24) & 0xFF) / 255.f,
                ((rgb >> 16) & 0xFF) / 255.f,
                ((rgb >> 8) & 0xFF) / 255.f,
                (rgb & 0xFF) / 255.f
            };
        }
        return std::nullopt;
    }
    if (len > 4 && c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == 'a' && c[4] == '(') {
        for (size_t i = 5 ; i < len - 1 ; i++) {
            if (!((c[i] >= '0' && c[i] <= '9') ||
            (c[i] == ',') ||
            (c[i] == ')') ||
            (c[i] == ' '))) return std::nullopt;
        }
        c+=5;
        int commaCount = 0;
        bool isValid = false;
        int r = parseCharValue(c,isValid);
        skipAllowed(c,commaCount);
        int g = parseCharValue(c,isValid);
        skipAllowed(c,commaCount);
        int b = parseCharValue(c,isValid);
        skipAllowed(c,commaCount);
        int a = parseCharValue(c,isValid);

        //Legacy css does not support all spaces
        if (commaCount < 3 || commaCount > 3 || !isValid) return std::nullopt;

        return SkColor4f{
            (float)r / 255.f,
            (float)g / 255.f,
            (float)b / 255.f,
            (float)a / 255.f,
        };
    }
    if (len > 3 && c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == '(') {
        for (size_t i = 4 ; i < len - 1 ; i++) {
            if (!((c[i] >= '0' && c[i] <= '9') ||
            (c[i] == ',') ||
            (c[i] == ')') ||
            (c[i] == ' '))) return std::nullopt;
        }
        if (c[len-1] != ')') return std::nullopt;
        c+=4;
        int commaCount = 0;
        bool isValid = false;
        int r = parseCharValue(c,isValid);
        skipAllowed(c,commaCount);
        int g = parseCharValue(c,isValid);
        skipAllowed(c,commaCount);
        int b = parseCharValue(c,isValid);

        if (commaCount != 2 && commaCount != 0 || !isValid) return std::nullopt;

        return SkColor4f{
            (float)r / 255.f,
            (float)g / 255.f,
            (float)b / 255.f,
            1.0f
        };
    }
    return std::nullopt;
}
