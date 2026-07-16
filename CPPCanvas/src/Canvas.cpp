SkPaint clearColor;
SkPaint noAlphaClearColor;
SkPaint pImageDataColor;

std::unordered_map<std::string,SkBlendMode> compositeOperations{{
    {"source-over",SkBlendMode::kSrcOver},
    {"source-in",SkBlendMode::kSrcIn},
    {"source-out",SkBlendMode::kSrcOut},
    {"source-atop",SkBlendMode::kSrcATop},
    {"destination-over",SkBlendMode::kDstOver},
    {"destination-in",SkBlendMode::kDstIn},
    {"destination-out",SkBlendMode::kDstOut},
    {"destination-atop",SkBlendMode::kDstATop},
    {"lighter",SkBlendMode::kLighten},
    {"copy",SkBlendMode::kSrc},
    {"xor",SkBlendMode::kXor},
    {"multiply",SkBlendMode::kMultiply},
    {"screen",SkBlendMode::kScreen},
    {"overlay",SkBlendMode::kOverlay},
    {"darken",SkBlendMode::kDarken},
    {"lighten",SkBlendMode::kLighten},
    {"color-dodge",SkBlendMode::kColorDodge},
    {"color-burn",SkBlendMode::kColorBurn},
    {"hard-light",SkBlendMode::kHardLight},
    {"soft-light",SkBlendMode::kSoftLight},
    {"difference",SkBlendMode::kDifference},
    {"exclusion",SkBlendMode::kExclusion},
    {"hue",SkBlendMode::kHue},
    {"saturation",SkBlendMode::kSaturation},
    {"color",SkBlendMode::kColor},
    {"luminosity",SkBlendMode::kLuminosity},
}};

std::unordered_map<std::string,SkColor4f> keywordColors{{
    {"aliceblue",SkColor4f{0.9411765f, 0.9725490f, 1.0000000f, 1.0f}},
    {"antiquewhite",SkColor4f{0.9803922f, 0.9215686f, 0.8431373f, 1.0f}},
    {"aqua",SkColor4f{0.0000000f, 1.0000000f, 1.0000000f, 1.0f}},
    {"aquamarine",SkColor4f{0.4980392f, 1.0000000f, 0.8313725f, 1.0f}},
    {"azure",SkColor4f{0.9411765f, 1.0000000f, 1.0000000f, 1.0f}},
    {"beige",SkColor4f{0.9607843f, 0.9607843f, 0.8627451f, 1.0f}},
    {"bisque",SkColor4f{1.0000000f, 0.8941176f, 0.7686275f, 1.0f}},
    {"black",SkColor4f{0.0000000f, 0.0000000f, 0.0000000f, 1.0f}},
    {"blanchedalmond",SkColor4f{1.0000000f, 0.9215686f, 0.8039216f, 1.0f}},
    {"blue",SkColor4f{0.0000000f, 0.0000000f, 1.0000000f, 1.0f}},
    {"blueviolet",SkColor4f{0.5411765f, 0.1686275f, 0.8862745f, 1.0f}},
    {"brown",SkColor4f{0.6470588f, 0.1647059f, 0.1647059f, 1.0f}},
    {"burlywood",SkColor4f{0.8705882f, 0.7215686f, 0.5294118f, 1.0f}},
    {"cadetblue",SkColor4f{0.3725490f, 0.6196078f, 0.6274510f, 1.0f}},
    {"chartreuse",SkColor4f{0.4980392f, 1.0000000f, 0.0000000f, 1.0f}},
    {"chocolate",SkColor4f{0.8235294f, 0.4117647f, 0.1176471f, 1.0f}},
    {"coral",SkColor4f{1.0000000f, 0.4980392f, 0.3137255f, 1.0f}},
    {"cornflowerblue",SkColor4f{0.3921569f, 0.5843137f, 0.9294118f, 1.0f}},
    {"cornsilk",SkColor4f{1.0000000f, 0.9725490f, 0.8627451f, 1.0f}},
    {"crimson",SkColor4f{0.8627451f, 0.0784314f, 0.2352941f, 1.0f}},
    {"cyan",SkColor4f{0.0000000f, 1.0000000f, 1.0000000f, 1.0f}},
    {"darkblue",SkColor4f{0.0000000f, 0.0000000f, 0.5450980f, 1.0f}},
    {"darkcyan",SkColor4f{0.0000000f, 0.5450980f, 0.5450980f, 1.0f}},
    {"darkgoldenrod",SkColor4f{0.7215686f, 0.5254902f, 0.0431373f, 1.0f}},
    {"darkgray",SkColor4f{0.6627451f, 0.6627451f, 0.6627451f, 1.0f}},
    {"darkgreen",SkColor4f{0.0000000f, 0.3921569f, 0.0000000f, 1.0f}},
    {"darkgrey",SkColor4f{0.6627451f, 0.6627451f, 0.6627451f, 1.0f}},
    {"darkkhaki",SkColor4f{0.7411765f, 0.7176471f, 0.4196078f, 1.0f}},
    {"darkmagenta",SkColor4f{0.5450980f, 0.0000000f, 0.5450980f, 1.0f}},
    {"darkolivegreen",SkColor4f{0.3333333f, 0.4196078f, 0.1843137f, 1.0f}},
    {"darkorange",SkColor4f{1.0000000f, 0.5490196f, 0.0000000f, 1.0f}},
    {"darkorchid",SkColor4f{0.6000000f, 0.1960784f, 0.8000000f, 1.0f}},
    {"darkred",SkColor4f{0.5450980f, 0.0000000f, 0.0000000f, 1.0f}},
    {"darksalmon",SkColor4f{0.9137255f, 0.5882353f, 0.4784314f, 1.0f}},
    {"darkseagreen",SkColor4f{0.5607843f, 0.7372549f, 0.5450980f, 1.0f}},
    {"darkslateblue",SkColor4f{0.2823530f, 0.2392157f, 0.5450980f, 1.0f}},
    {"darkslategray",SkColor4f{0.1843137f, 0.3098039f, 0.3098039f, 1.0f}},
    {"darkslategrey",SkColor4f{0.1843137f, 0.3098039f, 0.3098039f, 1.0f}},
    {"darkturquoise",SkColor4f{0.0000000f, 0.8078431f, 0.8196078f, 1.0f}},
    {"darkviolet",SkColor4f{0.5803922f, 0.0000000f, 0.8274510f, 1.0f}},
    {"deeppink",SkColor4f{1.0000000f, 0.0784314f, 0.5764706f, 1.0f}},
    {"deepskyblue",SkColor4f{0.0000000f, 0.7490196f, 1.0000000f, 1.0f}},
    {"dimgray",SkColor4f{0.4117647f, 0.4117647f, 0.4117647f, 1.0f}},
    {"dimgrey",SkColor4f{0.4117647f, 0.4117647f, 0.4117647f, 1.0f}},
    {"dodgerblue",SkColor4f{0.1176471f, 0.5647059f, 1.0000000f, 1.0f}},
    {"firebrick",SkColor4f{0.6980392f, 0.1333333f, 0.1333333f, 1.0f}},
    {"floralwhite",SkColor4f{1.0000000f, 0.9803922f, 0.9411765f, 1.0f}},
    {"forestgreen",SkColor4f{0.1333333f, 0.5450980f, 0.1333333f, 1.0f}},
    {"fuchsia",SkColor4f{1.0000000f, 0.0000000f, 1.0000000f, 1.0f}},
    {"gainsboro",SkColor4f{0.8627451f, 0.8627451f, 0.8627451f, 1.0f}},
    {"ghostwhite",SkColor4f{0.9725490f, 0.9725490f, 1.0000000f, 1.0f}},
    {"gold",SkColor4f{1.0000000f, 0.8431373f, 0.0000000f, 1.0f}},
    {"goldenrod",SkColor4f{0.8549020f, 0.6470588f, 0.1254902f, 1.0f}},
    {"gray",SkColor4f{0.5019608f, 0.5019608f, 0.5019608f, 1.0f}},
    {"green",SkColor4f{0.0000000f, 0.5019608f, 0.0000000f, 1.0f}},
    {"greenyellow",SkColor4f{0.6784314f, 1.0000000f, 0.1843137f, 1.0f}},
    {"grey",SkColor4f{0.5019608f, 0.5019608f, 0.5019608f, 1.0f}},
    {"honeydew",SkColor4f{0.9411765f, 1.0000000f, 0.9411765f, 1.0f}},
    {"hotpink",SkColor4f{1.0000000f, 0.4117647f, 0.7058824f, 1.0f}},
    {"indianred",SkColor4f{0.8039216f, 0.3607843f, 0.3607843f, 1.0f}},
    {"indigo",SkColor4f{0.2941176f, 0.0000000f, 0.5098039f, 1.0f}},
    {"ivory",SkColor4f{1.0000000f, 1.0000000f, 0.9411765f, 1.0f}},
    {"khaki",SkColor4f{0.9411765f, 0.9019608f, 0.5490196f, 1.0f}},
    {"lavender",SkColor4f{0.9019608f, 0.9019608f, 0.9803922f, 1.0f}},
    {"lavenderblush",SkColor4f{1.0000000f, 0.9411765f, 0.9607843f, 1.0f}},
    {"lawngreen",SkColor4f{0.4862745f, 0.9882353f, 0.0000000f, 1.0f}},
    {"lemonchiffon",SkColor4f{1.0000000f, 0.9803922f, 0.8039216f, 1.0f}},
    {"lightblue",SkColor4f{0.6784314f, 0.8470588f, 0.9019608f, 1.0f}},
    {"lightcoral",SkColor4f{0.9411765f, 0.5019608f, 0.5019608f, 1.0f}},
    {"lightcyan",SkColor4f{0.8784314f, 1.0000000f, 1.0000000f, 1.0f}},
    {"lightgoldenrodyellow",SkColor4f{0.9803922f, 0.9803922f, 0.8235294f, 1.0f}},
    {"lightgray",SkColor4f{0.8274510f, 0.8274510f, 0.8274510f, 1.0f}},
    {"lightgreen",SkColor4f{0.5647059f, 0.9333333f, 0.5647059f, 1.0f}},
    {"lightgrey",SkColor4f{0.8274510f, 0.8274510f, 0.8274510f, 1.0f}},
    {"lightpink",SkColor4f{1.0000000f, 0.7137255f, 0.7568628f, 1.0f}},
    {"lightsalmon",SkColor4f{1.0000000f, 0.6274510f, 0.4784314f, 1.0f}},
    {"lightseagreen",SkColor4f{0.1254902f, 0.6980392f, 0.6666667f, 1.0f}},
    {"lightskyblue",SkColor4f{0.5294118f, 0.8078431f, 0.9803922f, 1.0f}},
    {"lightslategray",SkColor4f{0.4666667f, 0.5333333f, 0.6000000f, 1.0f}},
    {"lightslategrey",SkColor4f{0.4666667f, 0.5333333f, 0.6000000f, 1.0f}},
    {"lightsteelblue",SkColor4f{0.6901961f, 0.7686275f, 0.8705882f, 1.0f}},
    {"lightyellow",SkColor4f{1.0000000f, 1.0000000f, 0.8784314f, 1.0f}},
    {"lime",SkColor4f{0.0000000f, 1.0000000f, 0.0000000f, 1.0f}},
    {"limegreen",SkColor4f{0.1960784f, 0.8039216f, 0.1960784f, 1.0f}},
    {"linen",SkColor4f{0.9803922f, 0.9411765f, 0.9019608f, 1.0f}},
    {"magenta",SkColor4f{1.0000000f, 0.0000000f, 1.0000000f, 1.0f}},
    {"maroon",SkColor4f{0.5019608f, 0.0000000f, 0.0000000f, 1.0f}},
    {"mediumaquamarine",SkColor4f{0.4000000f, 0.8039216f, 0.6666667f, 1.0f}},
    {"mediumblue",SkColor4f{0.0000000f, 0.0000000f, 0.8039216f, 1.0f}},
    {"mediumorchid",SkColor4f{0.7294118f, 0.3333333f, 0.8274510f, 1.0f}},
    {"mediumpurple",SkColor4f{0.5764706f, 0.4392157f, 0.8588235f, 1.0f}},
    {"mediumseagreen",SkColor4f{0.2352941f, 0.7019608f, 0.4431373f, 1.0f}},
    {"mediumslateblue",SkColor4f{0.4823529f, 0.4078431f, 0.9333333f, 1.0f}},
    {"mediumspringgreen",SkColor4f{0.0000000f, 0.9803922f, 0.6039216f, 1.0f}},
    {"mediumturquoise",SkColor4f{0.2823530f, 0.8196078f, 0.8000000f, 1.0f}},
    {"mediumvioletred",SkColor4f{0.7803922f, 0.0823529f, 0.5215686f, 1.0f}},
    {"midnightblue",SkColor4f{0.0980392f, 0.0980392f, 0.4392157f, 1.0f}},
    {"mintcream",SkColor4f{0.9607843f, 1.0000000f, 0.9803922f, 1.0f}},
    {"mistyrose",SkColor4f{1.0000000f, 0.8941176f, 0.8823529f, 1.0f}},
    {"moccasin",SkColor4f{1.0000000f, 0.8941176f, 0.7098039f, 1.0f}},
    {"navajowhite",SkColor4f{1.0000000f, 0.8705882f, 0.6784314f, 1.0f}},
    {"navy",SkColor4f{0.0000000f, 0.0000000f, 0.5019608f, 1.0f}},
    {"oldlace",SkColor4f{0.9921569f, 0.9607843f, 0.9019608f, 1.0f}},
    {"olive",SkColor4f{0.5019608f, 0.5019608f, 0.0000000f, 1.0f}},
    {"olivedrab",SkColor4f{0.4196078f, 0.5568627f, 0.1372549f, 1.0f}},
    {"orange",SkColor4f{1.0000000f, 0.6470588f, 0.0000000f, 1.0f}},
    {"orangered",SkColor4f{1.0000000f, 0.2705882f, 0.0000000f, 1.0f}},
    {"orchid",SkColor4f{0.8549020f, 0.4392157f, 0.8392157f, 1.0f}},
    {"palegoldenrod",SkColor4f{0.9333333f, 0.9098039f, 0.6666667f, 1.0f}},
    {"palegreen",SkColor4f{0.5960784f, 0.9843137f, 0.5960784f, 1.0f}},
    {"paleturquoise",SkColor4f{0.6862745f, 0.9333333f, 0.9333333f, 1.0f}},
    {"palevioletred",SkColor4f{0.8588235f, 0.4392157f, 0.5764706f, 1.0f}},
    {"papayawhip",SkColor4f{1.0000000f, 0.9372549f, 0.8352941f, 1.0f}},
    {"peachpuff",SkColor4f{1.0000000f, 0.8549020f, 0.7254902f, 1.0f}},
    {"peru",SkColor4f{0.8039216f, 0.5215686f, 0.2470588f, 1.0f}},
    {"pink",SkColor4f{1.0000000f, 0.7529412f, 0.7960784f, 1.0f}},
    {"plum",SkColor4f{0.8666667f, 0.6274510f, 0.8666667f, 1.0f}},
    {"powderblue",SkColor4f{0.6901961f, 0.8784314f, 0.9019608f, 1.0f}},
    {"purple",SkColor4f{0.5019608f, 0.0000000f, 0.5019608f, 1.0f}},
    {"rebeccapurple",SkColor4f{0.4000000f, 0.2000000f, 0.6000000f, 1.0f}},
    {"red",SkColor4f{1.0000000f, 0.0000000f, 0.0000000f, 1.0f}},
    {"rosybrown",SkColor4f{0.7372549f, 0.5607843f, 0.5607843f, 1.0f}},
    {"royalblue",SkColor4f{0.2549020f, 0.4117647f, 0.8823529f, 1.0f}},
    {"saddlebrown",SkColor4f{0.5450980f, 0.2705882f, 0.0745098f, 1.0f}},
    {"salmon",SkColor4f{0.9803922f, 0.5019608f, 0.4470588f, 1.0f}},
    {"sandybrown",SkColor4f{0.9568627f, 0.6431373f, 0.3764706f, 1.0f}},
    {"seagreen",SkColor4f{0.1803922f, 0.5450980f, 0.3411765f, 1.0f}},
    {"seashell",SkColor4f{1.0000000f, 0.9607843f, 0.9333333f, 1.0f}},
    {"sienna",SkColor4f{0.6274510f, 0.3215686f, 0.1764706f, 1.0f}},
    {"silver",SkColor4f{0.7529412f, 0.7529412f, 0.7529412f, 1.0f}},
    {"skyblue",SkColor4f{0.5294118f, 0.8078431f, 0.9215686f, 1.0f}},
    {"slateblue",SkColor4f{0.4156863f, 0.3529412f, 0.8039216f, 1.0f}},
    {"slategray",SkColor4f{0.4392157f, 0.5019608f, 0.5647059f, 1.0f}},
    {"slategrey",SkColor4f{0.4392157f, 0.5019608f, 0.5647059f, 1.0f}},
    {"snow",SkColor4f{1.0000000f, 0.9803922f, 0.9803922f, 1.0f}},
    {"springgreen",SkColor4f{0.0000000f, 1.0000000f, 0.4980392f, 1.0f}},
    {"steelblue",SkColor4f{0.2745098f, 0.5098039f, 0.7058824f, 1.0f}},
    {"tan",SkColor4f{0.8235294f, 0.7058824f, 0.5490196f, 1.0f}},
    {"teal",SkColor4f{0.0000000f, 0.5019608f, 0.5019608f, 1.0f}},
    {"thistle",SkColor4f{0.8470588f, 0.7490196f, 0.8470588f, 1.0f}},
    {"tomato",SkColor4f{1.0000000f, 0.3882353f, 0.2784314f, 1.0f}},
    {"transparent",SkColor4f{0.0f, 0.0f, 0.0f, 0.0f}},
    {"turquoise",SkColor4f{0.2509804f, 0.8784314f, 0.8156863f, 1.0f}},
    {"violet",SkColor4f{0.9333333f, 0.5098039f, 0.9333333f, 1.0f}},
    {"wheat",SkColor4f{0.9607843f, 0.8705882f, 0.7019608f, 1.0f}},
    {"white",SkColor4f{1.0000000f, 1.0000000f, 1.0000000f, 1.0f}},
    {"whitesmoke",SkColor4f{0.9607843f, 0.9607843f, 0.9607843f, 1.0f}},
    {"yellow",SkColor4f{1.0000000f, 1.0000000f, 0.0000000f, 1.0f}},
    {"yellowgreen",SkColor4f{0.6039216f, 0.8039216f, 0.1960784f, 1.0f}},
}};



class BunCanvas;

enum Rendering2DContextSettings {
    A = 0
};

class BunCanvasRenderingContext2D {
    SkCanvas* ctx = nullptr;
    public:
    BunCanvas* owner;
    SkPathBuilder pathBuilder;
    SkPaint fillColor;
    SkPaint strokeColor;
    SkPaint imageColor;
    static constexpr uint64_t MAGIC = 0x5E5A8750;
    uint64_t magic = MAGIC;
    std::string cssFont;
    CanvasFontCache font_cache;
    SkFont font_current;
    
    SkSamplingOptions sampling;
    bool locked = false;

    float globalAlpha = 1.f;
    
    BunCanvasRenderingContext2D(sk_sp<SkSurface>& surface, BunCanvas* owner) : ctx(surface->getCanvas()),sampling(SkFilterMode::kLinear){
        this->owner = owner;
        strokeColor.setColor(SK_ColorBLACK);
        strokeColor.setStyle(SkPaint::kStroke_Style);
        strokeColor.setAlpha(255);
        strokeColor.setStrokeWidth(1);
        strokeColor.setAntiAlias(1);
        strokeColor.setBlendMode(compositeOperations.at("source-over"));
        
        imageColor.setColor(SK_ColorWHITE);
        imageColor.setAlpha(255);
        imageColor.setStyle(SkPaint::kFill_Style);
        imageColor.setAntiAlias(1);
        imageColor.setBlendMode(compositeOperations.at("source-over"));
        
        fillColor.setColor(SK_ColorBLACK);
        fillColor.setStyle(SkPaint::kFill_Style);
        fillColor.setAlpha(255);
        fillColor.setAntiAlias(1);
        fillColor.setBlendMode(compositeOperations.at("source-over"));
    }
    
    //Mimicking the behavior of values reset when resizing a canvas object.
    void reset(sk_sp<SkSurface>& surface) {
        // std::lock_guard<std::mutex> lock(draw_mutex);
        ctx = surface->getCanvas();
        ctx->resetMatrix();
        pathBuilder.reset();
        fillColor.setBlendMode(compositeOperations.at("source-over"));
        fillColor.setColor(SK_ColorBLACK);
        fillColor.setStyle(SkPaint::kFill_Style);
        fillColor.setAntiAlias(1);
        
        strokeColor.setBlendMode(compositeOperations.at("source-over"));
        strokeColor.setColor(SK_ColorBLACK);
        strokeColor.setStyle(SkPaint::kStroke_Style);
        strokeColor.setStrokeWidth(1);
        strokeColor.setAntiAlias(1);
        
        imageColor.setBlendMode(compositeOperations.at("source-over"));

        cssFont = "10px sans-serif";               // e.g. "20px 700 italic Arial, sans-serif"

        if (!font_cache.setFont(cssFont.c_str(),font_current)){
            std::cout << "This somehow failed! (It shouldn't because it is the default)\n";
        }
        
        
        // sk_sp<SkTypeface> tf;
        // SkFontStyle style;
        // {
        //     std::lock_guard<std::mutex> lk(fontCacheMtx);
        //     auto it = fontCache.find(cssFont);
        //     if (it != fontCache.end()) {
        //         tf = it->second;
        //     } else {
        //         // Ask the platform FontMgr for the best match.
        //         // We reuse the same parsing helpers used above.
        //         // inside the cache‑miss branch
        //         style = parseCssToSkFontStyle(cssFont);
        //         std::string family = parseCssFamilyList(cssFont);
                            
        //         // Use a harmless default character (space) when no particular glyph is required.
        //         const SkUnichar kDefaultChar = 0x20;   // ' '
                            
        //         tf = fontMgr->matchFamilyStyleCharacter(
        //                 family.c_str(),
        //                 style,
        //                 nullptr,   // bcp47 array
        //                 0,         // bcp47Count
        //                 kDefaultChar);   // <-- added 5th argument
        //         if (!tf) tf = fontMgr->matchFamilyStyle(nullptr, style); // fallback
        //         fontCache.emplace(cssFont, tf);
        //     }
        // }
        // ---- 3.3 Install into the SkFont used for drawing ----
        // font.setTypeface(tf);
        // font.setSize(/*sizePx parsed from key*/ parseCssFontSize(cssFont)); // placeholder – real parse needed
    }
    
    SkCanvas* operator()() {
        return ctx;
    }
};

struct RetiredTexture {
    GrBackendTexture tex;
    int framesLeft;
};

class BunCanvas {
    std::string ctxType;
    BunCanvasRenderingContext2D* rendering2D = nullptr;
    public:
    
    static constexpr uint64_t MAGIC = 0xBCA1155A;
    uint64_t magic = MAGIC;
    sk_sp<SkSurface> surface;
    std::mutex mutex;
    
    #ifndef __APPLE__
    GrBackendTexture backendTex;
    bool hasBackendTex = false;
    std::vector<RetiredTexture> retiredTextures;
    #endif
    
    
    BunCanvas(int w, int h){
        nonapple(std::lock_guard lock(mutex));
        #ifdef __APPLE__
        if (renderThreadContext == nullptr){
            surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w,h));
        }else {
            surface = SkSurfaces::RenderTarget(renderThreadContext->context.get(), skgpu::Budgeted::kYes, SkImageInfo::MakeN32Premul(w,h));
        }
        #else
        if (gpuContextReady) {
            backendTex = mainThreadCtx->context->createBackendTexture(
                w, h,
                kN32_SkColorType,
                skgpu::Mipmapped::kNo,
                GrRenderable::kYes,
                GrProtected::kNo
            );
            
            if (backendTex.isValid()) {
                surface = SkSurfaces::WrapBackendTexture(
                    mainThreadCtx->context.get(),
                    backendTex,
                    kTopLeft_GrSurfaceOrigin,
                    1,
                    kN32_SkColorType,
                    nullptr,
                    nullptr
                );
                hasBackendTex = true;
            }
        }
        if (!surface) {
            std::cout << "rasted used!\n";
            surface = SkSurfaces::Raster(
                SkImageInfo::MakeN32Premul(w,h)
            );
            hasBackendTex = false;
        }
        #endif
    }
    
    //Maybe future support for webGPU rendering context if possible
    ~BunCanvas(){
        delete rendering2D;
        surface.reset();
        
        #ifndef __APPLE__
        // Flush to ensure no pending operations on the current texture
        if (hasBackendTex && mainThreadCtx) {
            mainThreadCtx->context->flushAndSubmit(GrSyncCpu::kYes);
            mainThreadCtx->context->deleteBackendTexture(backendTex);
        }
        // Clean up any retired textures immediately
        for (auto& r : retiredTextures) {
            GrGLTextureInfo info;
            if (GrBackendTextures::GetGLTextureInfo(r.tex, &info)) {
                glDeleteTextures(1, &info.fID);
            }
        }
        retiredTextures.clear();
        #endif
    }

    void* getContext(const char* c) {
        if (std::strcmp("2d",ctxType.c_str()) == 0) return rendering2D;
        if (std::strcmp("2d",c) == 0) {
            if (rendering2D == nullptr) rendering2D = new BunCanvasRenderingContext2D(surface,this);
            ctxType = c;
            return rendering2D;
        }
        return nullptr;
    }

    void resize(int w, int h) {
        if (rendering2D == nullptr) return;

        #ifndef __APPLE__
        // Flush main thread GPU commands and wait for completion
        // before retiring the old texture
        if (hasBackendTex && mainThreadCtx) {
            mainThreadCtx->context->flushAndSubmit(GrSyncCpu::kYes);
            // Retire the texture instead of deleting it immediately.
            // The render thread might still have pending draw commands
            // that reference this texture from the previous frame.
            retiredTextures.push_back({backendTex, 3});
            hasBackendTex = false;
        }
        #endif
        
        surface.reset();

        #ifdef __APPLE__
        if (renderThreadContext == nullptr){
            surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w,h));
        }else {
            surface = SkSurfaces::RenderTarget(renderThreadContext->context.get(), skgpu::Budgeted::kYes, SkImageInfo::MakeN32Premul(w,h));
        }
        #else
        if (gpuContextReady) {
            backendTex = mainThreadCtx->context->createBackendTexture(
                w, h,
                kN32_SkColorType,
                skgpu::Mipmapped::kNo,
                GrRenderable::kYes,
                GrProtected::kNo
            );
            if (backendTex.isValid()) {
                surface = SkSurfaces::WrapBackendTexture(
                    mainThreadCtx->context.get(),
                    backendTex,
                    kTopLeft_GrSurfaceOrigin,
                    1,
                    kN32_SkColorType,
                    nullptr,
                    nullptr
                );
                hasBackendTex = true;
            }
        }
        if (!surface) {
            surface = SkSurfaces::Raster(
                SkImageInfo::MakeN32Premul(w,h)
            );
            hasBackendTex = false;
        }
        #endif
        
        if (ctxType == "2d") rendering2D->reset(surface);
    }
};


std::vector<BunCanvas*> canvases;

extern "C" {
    
    WINDOWS_EXPORT void* canvas_create(int w, int h) {
        return new BunCanvas(w,h);
    }
    
    WINDOWS_EXPORT void canvas_append(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvas* obj = static_cast<BunCanvas*>(canvasObj);
        if (obj->magic != BunCanvas::MAGIC)
        return;
        
        canvases.push_back(obj);
    }
    
    //Sets native internal context so it is returned if is asked again.
    WINDOWS_EXPORT void* canvas_setup_context(void* canvasObj, const char* ctxType){
        if (!canvasObj) return nullptr;
        BunCanvas* obj = validated<BunCanvas>(canvasObj);
        
        if (obj == nullptr) return nullptr;
        
        void* ptr = obj->getContext(ctxType);
        
        return ptr;
        
        // obj->ctx = obj->surface->getCanvas();
    }
    
    #define propertyColor(methodName,target) bool methodName (void* renderingContext, const char* c) {\
        if (!renderingContext) return false;\
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(renderingContext);\
        if (obj == nullptr)\
        return false;\
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex);)\
        try {\
            target.setColor(keywordColors.at(c));\
            return true;\
        }catch (std::exception){\
            if (obj == nullptr) return false;\
            if (c[0] == '#') {\
                if (std::strlen(c) == 7) {\
                    unsigned int rgb = std::strtoul(c+1, nullptr, 16);\
                    target.setColor(SkColor4f{\
                        ((rgb >> 16) & 0xFF) / 255.f,\
                        ((rgb >> 8) & 0xFF) / 255.f,\
                        (rgb & 0xFF) / 255.f,\
                        1.0f\
                    });\
                    return true;\
                }\
                if (std::strlen(c) == 9) {\
                    unsigned int rgb = std::strtoul(c+1, nullptr, 16);\
                    target.setColor(SkColor4f{\
                        ((rgb >> 24) & 0xFF) / 255.f,\
                        ((rgb >> 16) & 0xFF) / 255.f,\
                        ((rgb >> 8) & 0xFF) / 255.f,\
                        (rgb & 0xFF) / 255.f\
                    });\
                    return true;\
                }\
                return false;\
            }\
            if (c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == 'a' && c[4] == '(') {\
                c+=5;\
                int r = 0;\
                while (*c >= '0' && *c <= '9') {\
                    r = r * 10 + (*c - '0');\
                    ++c;\
                }\
                r = std::max(r,0);\
                r = std::min(r,255);\
                while (*c == ' ' || *c == ',') ++c;\
                int g = 0;\
                while (*c >= '0' && *c <= '9') {\
                    g = g * 10 + (*c - '0');\
                    ++c;\
                }\
                g = std::max(g,0);\
                g = std::min(g,255);\
                while (*c == ' ' || *c == ',') ++c;\
                int b = 0;\
                while (*c >= '0' && *c <= '9') {\
                    b = b * 10 + (*c - '0');\
                    ++c;\
                }\
                b = std::min(std::max(b,0),255);\
                target.setColor(SkColor4f{\
                    (float)r / 255.f,\
                    (float)g / 255.f,\
                    (float)b / 255.f,\
                    1.0f\
                });\
                while (*c == ' ' || *c == ',') ++c;\
                int a = 0;\
                while (*c >= '0' && *c <= '9') {\
                    a = a * 10 + (*c - '0');\
                    ++c;\
                }\
                a = std::min(std::max(a,0),255);\
                target.setColor(SkColor4f{\
                    (float)r / 255.f,\
                    (float)g / 255.f,\
                    (float)b / 255.f,\
                    (float)a / 255.f,\
                });\
                return true;\
            }\
            if (c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == '(') {\
                c+=4;\
                int r = 0;\
                while (*c >= '0' && *c <= '9') {\
                    r = r * 10 + (*c - '0');\
                    ++c;\
                }\
                r = std::max(r,0);\
                r = std::min(r,255);\
                while (*c == ' ' || *c == ',') ++c;\
                int g = 0;\
                while (*c >= '0' && *c <= '9') {\
                    g = g * 10 + (*c - '0');\
                    ++c;\
                }\
                g = std::max(g,0);\
                g = std::min(g,255);\
                while (*c == ' ' || *c == ',') ++c;\
                int b = 0;\
                while (*c >= '0' && *c <= '9') {\
                    b = b * 10 + (*c - '0');\
                    ++c;\
                }\
                b = std::max(b,0);\
                b = std::min(b,255);\
                target.setColor(SkColor4f{\
                    (float)r / 255.f,\
                    (float)g / 255.f,\
                    (float)b / 255.f,\
                    1.0f\
                });\
                return true;\
            }\
        }\
        return false;\
    }
    
    WINDOWS_EXPORT propertyColor(canvas_set_fill_style,obj->fillColor)
    WINDOWS_EXPORT propertyColor(canvas_set_stroke_style,obj->strokeColor)
    
    WINDOWS_EXPORT void canvas_set_stroke_width(void* canvasObj, float w) {
        if (!canvasObj) return;
        
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->strokeColor.setStrokeWidth(w);
    }
    
    WINDOWS_EXPORT void canvas_fill_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        SkPaint fColor = obj->fillColor;
        fColor.setAlphaf(obj->fillColor.getAlphaf()*obj->globalAlpha);
        (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), fColor);
    }
    
    WINDOWS_EXPORT void canvas_clear_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;
        
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        if (!obj) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), clearColor);
    }
    
    WINDOWS_EXPORT void canvas_resize(void* canvasObj, int w, int h) {
        if (!canvasObj) return;
        BunCanvas* obj = validated<BunCanvas>(canvasObj);
        
        if (obj == nullptr) return;
        
        {
            nonapple(std::lock_guard<std::mutex> lock(obj->mutex));
            obj->resize(w,h);
        }
    }
    
    WINDOWS_EXPORT void canvas_path_begin(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->pathBuilder.reset();
    }
    
    WINDOWS_EXPORT void canvas_path_move_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->pathBuilder.moveTo(x,y);
    }
    WINDOWS_EXPORT void canvas_path_line_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->pathBuilder.lineTo(x,y);
    }
    
    WINDOWS_EXPORT void canvas_path_arc(void* canvasObj, float x1, float y1, float radius, float startAngle, float sweepangle) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->pathBuilder.addArc(SkRect::MakeXYWH(x1 - radius,y1 - radius,radius * 2,radius * 2), startAngle*57.29577958f, sweepangle*57.29577958f);
    }
    
    WINDOWS_EXPORT void canvas_path_arc_to(void* canvasObj, float x1, float y1, float x2, float y2, float radius) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->pathBuilder.arcTo({x1,y1},{x2,y2},radius);
    }
    
    WINDOWS_EXPORT void canvas_path_bezier_to(void* canvasObj, float x1, float y1, float x2, float y2, float x3, float y3) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->pathBuilder.cubicTo(x1,y1,x2,y2,x3,y3);
    }
    WINDOWS_EXPORT void canvas_path_stroke(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) {
            return;
        };
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));

        SkPaint sColor = obj->strokeColor;
        auto alphaf = obj->strokeColor.getAlphaf()*obj->globalAlpha;
        sColor.setAlphaf(alphaf);
        (*obj)()->drawPath(obj->pathBuilder.snapshot(), sColor);
    }
    
    WINDOWS_EXPORT void canvas_path_close(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->pathBuilder.close();
    }
    
    WINDOWS_EXPORT void canvas_draw_image_imageType(void* canvasObj, void* image, float x,float y,float w,float h) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        ImageWrapper* img = static_cast<ImageWrapper*>(image);
        if (obj == nullptr || img == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        (*obj)()->drawImageRect(img->image.get(),SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(obj->imageColor));
    }

    WINDOWS_EXPORT void canvas_draw_image_canvasType(void* canvasObj, void* image, float x,float y,float w,float h) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        BunCanvas* img = validated<BunCanvas>(image);
        if (obj == nullptr || img == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        nonapple(std::lock_guard<std::mutex> lock2(img->mutex));
        #ifndef __APPLE__
        if (img->hasBackendTex) {
            auto _img = SkImages::BorrowTextureFrom(
                renderThreadContext->context.get(),
                img->backendTex,
                kTopLeft_GrSurfaceOrigin,
                kN32_SkColorType,
                kPremul_SkAlphaType,
                nullptr
            );
            if (_img) {
                (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(obj->imageColor));
            }
        } else {
            auto _img = img->surface->makeTemporaryImage();
            (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(obj->imageColor));
        }
        #else
        auto _img = img->surface->makeTemporaryImage();
        (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(obj->imageColor));
        #endif
    }
    
    WINDOWS_EXPORT bool canvas_set_composite_operation(void* canvasObj, const char* name) {
        if (!canvasObj) return false;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return false;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        try {
            obj->fillColor.setBlendMode(compositeOperations.at(name));
            obj->strokeColor.setBlendMode(compositeOperations.at(name));
            obj->imageColor.setBlendMode(compositeOperations.at(name));
            return true;
        }catch(std::exception err) {
            return false;
        }
    }
    
    WINDOWS_EXPORT bool canvas_get_image_data(void* canvasObj,int x, int y, int w, int h, uint8_t* out_buffer) {
        if (!canvasObj) return false;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        if (obj == nullptr) {
            return false;
        };
        {
            nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
            SkImageInfo dstInfo = SkImageInfo::Make(
                w,h,
                kRGBA_8888_SkColorType,
                kPremul_SkAlphaType
            );
            size_t rowBytes = w * 4;


            // if (obj->owner->hasBackendTex) renderThreadContext->context->flushAndSubmit();
            
            return (*obj)()->getSurface()->makeTemporaryImage()->readPixels(
                dstInfo, 
                out_buffer, 
                rowBytes, 
                x, 
                y, 
                SkImage::CachingHint::kDisallow_CachingHint
            );
        }
    }

    // WINDOWS_EXPORT bool canvas_get_image_data(void* canvasObj,int x, int y, int w, int h, uint8_t* out_buffer) {
    //     if (!canvasObj) return false;
    //     BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
    //     if (obj == nullptr) {
    //         return false;
    //     };
    //     {
    //         nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
    //         SkImageInfo dstInfo = SkImageInfo::Make(
    //             w,h,
    //             kRGBA_8888_SkColorType,
    //             kPremul_SkAlphaType
    //         );
    //         size_t rowBytes = w * 4;
            
    //         if (obj->owner->hasBackendTex) {
    //             auto _img = SkImages::BorrowTextureFrom(
    //                 renderThreadContext->context.get(),
    //                 obj->owner->backendTex,
    //                 kTopLeft_GrSurfaceOrigin,
    //                 kN32_SkColorType,
    //                 kPremul_SkAlphaType,
    //                 nullptr
    //             );
    //             if (_img) return _img->makeNonTextureImage()->readPixels(
    //                     dstInfo, 
    //                     out_buffer, 
    //                     rowBytes, 
    //                     x, 
    //                     y, 
    //                     SkImage::CachingHint::kDisallow_CachingHint
    //                 );
    //             return false;
    //         } else {
    //             auto _img = obj->owner->surface->makeTemporaryImage();
    //             // (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(obj->imageColor));
    //             if (_img) return _img->readPixels(
    //                 dstInfo, 
    //                 out_buffer, 
    //                 rowBytes, 
    //                 x, 
    //                 y, 
    //                 SkImage::CachingHint::kDisallow_CachingHint
    //             );

    //             return false;
    //         }
            
            
    //     }
    // }
    
    
    WINDOWS_EXPORT bool canvas_put_image_data(void* canvasObj, int x, int y, int w, int h, uint8_t* buffer){
        if (!canvasObj) return false;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return false;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        SkImageInfo imageInfo = SkImageInfo::Make(
            w,h,
            kRGBA_8888_SkColorType,
            kPremul_SkAlphaType
        );
        
        SkBitmap bitmap;

        
        if(bitmap.installPixels(imageInfo,buffer,w *4)){
            
            (*obj)()->drawImage(bitmap.asImage(),x,y,obj->sampling,&pImageDataColor);
            // std::cout << "Success " << x << " " << y << " " << w << " " << h << " " << "\n";
            return true;
        }
        return false;
    }
    WINDOWS_EXPORT float canvas_set_global_alpha(void* canvasObj, float a){
        float _a = std::clamp(a, 0.0f, 1.0f);
        if (!canvasObj) return _a;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        if (obj == nullptr) return _a;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->globalAlpha = _a;
        // obj->fillColor.setAlpha(_a);
        // obj->strokeColor.setAlpha(_a);
        obj->imageColor.setAlphaf(_a);
        return _a;
    }
    WINDOWS_EXPORT void canvas_save(void* canvasObj){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        (*obj)()->save();
    }
    WINDOWS_EXPORT void canvas_restore(void* canvasObj){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        (*obj)()->restore();
    }
    WINDOWS_EXPORT void canvas_translate(void* canvasObj, float x, float y){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        (*obj)()->translate(x,y);
    }
    WINDOWS_EXPORT void canvas_rotate(void* canvasObj, float deg){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        (*obj)()->rotate(deg);
    }
    
    WINDOWS_EXPORT bool canvas_set_font(void* ctxPtr, const char* cssString) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        return ctx->font_cache.setFont(cssString,ctx->font_current);
    }

    WINDOWS_EXPORT void canvas_fill_text(void* ctxPtr, const char* txt, float x, float y, float maxWidth) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        float width = ctx->font_current.measureText(
            txt,
            std::strlen(txt),
            SkTextEncoding::kUTF8
        );
        if (width > maxWidth && maxWidth > -1) {
            float scale = maxWidth / width;
        
            (*ctx)()->save();
            (*ctx)()->translate(x, y);
            (*ctx)()->scale(scale, 1.0f);
            (*ctx)()->drawString(txt, x, y, ctx->font_current, ctx->fillColor);
            (*ctx)()->restore();
        } else {
            (*ctx)()->drawString(txt, x, y, ctx->font_current, ctx->fillColor);
        }
    }
}