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
int parseCharValueAlpha(const char*& c, bool& isValid) {
    float n = 0.0f;
    bool v = false;

    // Integer part
    while (*c >= '0' && *c <= '9') {
        v = true;
        n = n * 10.0f + (*c - '0');
        ++c;
    }

    // Fractional part
    if (*c == '.') {
        ++c;

        float place = 0.1f;
        while (*c >= '0' && *c <= '9') {
            v = true;
            n += (*c - '0') * place;
            place *= 0.1f;
            ++c;
        }
    }

    isValid = v;

    return static_cast<int>(std::clamp(n, 0.0f, 1.0f) * 255.0f);
}

//Skips only a single comma and any number of spaces
void skipAllowed(const char*& c, int& commaCount){
    while (*c == ' ' || *c == ',') {
        if (*c == ',') commaCount++;
        ++c;
    };
}

static const uint8_t hexTable[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0-15
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 16-31
    // ... fill with 0-9, 0-9, A-F, a-f mappings
    ['0']=0, ['1']=1, ['2']=2, ['3']=3, ['4']=4,
    ['5']=5, ['6']=6, ['7']=7, ['8']=8, ['9']=9,
    ['A']=10, ['B']=11, ['C']=12, ['D']=13, ['E']=14, ['F']=15,
    ['a']=10, ['b']=11, ['c']=12, ['d']=13, ['e']=14, ['f']=15
};

std::optional<SkColor4f> parseCssColor(const char* c){
    if (!c) return std::nullopt;
    
    auto len = std::strlen(c);
    //Comparing length
    if (len == 4 && c[0] == '#') {
        uint8_t r = hexTable[(uint8_t)c[1]];
        uint8_t g = hexTable[(uint8_t)c[2]];
        uint8_t b = hexTable[(uint8_t)c[3]];
        return SkColor4f{
            ((r << 4) | r) / 255.f,
            ((g << 4) | g) / 255.f,
            ((b << 4) | b) / 255.f,
            1.0f
        };
        // unsigned int rgb = std::strtoul(c + 1, nullptr, 16);
        // uint8_t r = (rgb >> 8) & 0xF;
        // uint8_t g = (rgb >> 4) & 0xF;
        // uint8_t b = rgb & 0xF;
        // r = (r << 4) | r;
        // g = (g << 4) | g;
        // b = (b << 4) | b;
        // auto col = SkColor4f{
        //     r / 255.f,
        //     g / 255.f,
        //     b / 255.f,
        //     1.0f
        // };
        // colorCache.insert({c,col});
        // return col;
    } else if (len == 5 && c[0] == '#') {
        uint8_t r = hexTable[(uint8_t)c[1]];
        uint8_t g = hexTable[(uint8_t)c[2]];
        uint8_t b = hexTable[(uint8_t)c[3]];
        uint8_t a = hexTable[(uint8_t)c[4]];
        return SkColor4f{
            ((r << 4) | r) / 255.f,
            ((g << 4) | g) / 255.f,
            ((b << 4) | b) / 255.f,
            ((a << 4) | a) / 255.f
        };
        // unsigned int rgba = std::strtoul(c + 1, nullptr, 16);
        // uint8_t r = (rgba >> 12) & 0xF;
        // uint8_t g = (rgba >> 8) & 0xF;
        // uint8_t b = (rgba >> 4) & 0xF;
        // uint8_t a = rgba & 0xF;
        // r = (r << 4) | r;
        // g = (g << 4) | g;
        // b = (b << 4) | b;
        // a = (a << 4) | a;
        // auto col = SkColor4f{
        //     r / 255.f,
        //     g / 255.f,
        //     b / 255.f,
        //     a / 255.f
        // };
        // colorCache.insert({c,col});
        // return col;
    } else if (len == 7 && c[0] == '#') {
        unsigned int rgb = std::strtoul(c+1, nullptr, 16);
        auto col = SkColor4f{
            ((rgb >> 16) & 0xFF) / 255.f,
            ((rgb >> 8) & 0xFF) / 255.f,
            (rgb & 0xFF) / 255.f,
            1.0f
        };
        // colorCache.insert({c,col});
        return col;
    } else if (len == 9 && c[0] == '#') {
        unsigned int rgb = std::strtoul(c+1, nullptr, 16);
        auto col = SkColor4f{
            ((rgb >> 24) & 0xFF) / 255.f,
            ((rgb >> 16) & 0xFF) / 255.f,
            ((rgb >> 8) & 0xFF) / 255.f,
            (rgb & 0xFF) / 255.f
        };
        // colorCache.insert({c,col});
        return col;
    }
    // if (len > 1 && c[0] == '#') {
    //     //Check if valid before computing
    //     for (size_t i = 1 ; i < len; i++) {
    //         if (!(
    //             (c[i] >= '0' && c[i] <= '9') ||
    //             (c[i] >= 'a' && c[i] <= 'f') ||
    //             (c[i] >= 'A' && c[i] <= 'F')
    //         )) return std::nullopt;
    //     }
        
    //     return std::nullopt;
    // }
    {
        auto it = colorCache.find(c);
        if (it != colorCache.end()) {
            return it->second;
        };
    }
    if (len > 4 && c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == 'a' && c[4] == '(') {
        std::string beforeParse = c;
        for (size_t i = 5 ; i < len - 1 ; i++) {
            if (!((c[i] >= '0' && c[i] <= '9') ||
            (c[i] == '.') ||
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
        float a = parseCharValueAlpha(c,isValid);

        //Legacy css does not support all spaces
        if (commaCount < 3 || commaCount > 3 || !isValid) return std::nullopt;

        auto col = SkColor4f{
            (float)r / 255.f,
            (float)g / 255.f,
            (float)b / 255.f,
            (float)a / 255.f,
        };
        colorCache.insert({beforeParse,col});
        return col;
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

        auto col = SkColor4f{
            (float)r / 255.f,
            (float)g / 255.f,
            (float)b / 255.f,
            1.0f
        };
        colorCache.insert({c,col});
        return col;
    }
    return std::nullopt;
}
