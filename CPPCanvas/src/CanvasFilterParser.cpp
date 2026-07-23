#pragma once
namespace css {

// ============================================================================
// Filter Value Types
// ============================================================================

struct BlurParams {
    SkScalar radius = 0;  // in pixels
};

struct BrightnessParams {
    float amount = 1.0f;  // 0.0 to infinity, 1.0 = no change
};

struct ContrastParams {
    float amount = 1.0f;  // 0.0 to infinity, 1.0 = no change
};

struct SaturateParams {
    float amount = 1.0f;  // 0.0 to infinity, 1.0 = no change, 0.0 = grayscale
};

struct GrayscaleParams {
    float amount = 0.0f;  // 0.0 to 1.0, 0.0 = no change, 1.0 = full grayscale
};

struct HueRotateParams {
    float degrees = 0.0f;  // degrees, can be any value (mod 360)
};

struct InvertParams {
    float amount = 0.0f;  // 0.0 to 1.0, 0.0 = no change, 1.0 = full invert
};

struct OpacityParams {
    float amount = 1.0f;  // 0.0 to 1.0, 0.0 = transparent, 1.0 = opaque
};

struct SepiaParams {
    float amount = 0.0f;  // 0.0 to 1.0, 0.0 = no change, 1.0 = full sepia
};

struct DropShadowParams {
    SkScalar dx = 0;
    SkScalar dy = 0;
    SkScalar blurRadius = 0;
    SkColor color = SK_ColorBLACK;  // includes alpha
};

// ============================================================================
// Filter Variant
// ============================================================================

struct Filter {
    enum Type {
        Blur,
        Brightness,
        Contrast,
        Saturate,
        Grayscale,
        HueRotate,
        Invert,
        Opacity,
        Sepia,
        DropShadow
    };

    Type type;
    std::variant<
        BlurParams,
        BrightnessParams,
        ContrastParams,
        SaturateParams,
        GrayscaleParams,
        HueRotateParams,
        InvertParams,
        OpacityParams,
        SepiaParams,
        DropShadowParams
    > params;

    // Convenience accessors
    const BlurParams& blur() const { return std::get<BlurParams>(params); }
    const BrightnessParams& brightness() const { return std::get<BrightnessParams>(params); }
    const ContrastParams& contrast() const { return std::get<ContrastParams>(params); }
    const SaturateParams& saturate() const { return std::get<SaturateParams>(params); }
    const GrayscaleParams& grayscale() const { return std::get<GrayscaleParams>(params); }
    const HueRotateParams& hueRotate() const { return std::get<HueRotateParams>(params); }
    const InvertParams& invert() const { return std::get<InvertParams>(params); }
    const OpacityParams& opacity() const { return std::get<OpacityParams>(params); }
    const SepiaParams& sepia() const { return std::get<SepiaParams>(params); }
    const DropShadowParams& dropShadow() const { return std::get<DropShadowParams>(params); }
};

// ============================================================================
// Parser
// ============================================================================

class FilterParser {
public:
    struct ParseError {
        std::string message;
        size_t position;
    };

    std::variant<std::vector<Filter>, ParseError> parse(std::string_view input) {
        std::vector<Filter> result;
        m_input = input;
        m_pos = 0;
        m_length = input.length();

        skipWhitespace();
        
        while (m_pos < m_length) {
            auto filterOpt = parseFilter();
            if (!filterOpt.has_value()) {
                return ParseError{"Failed to parse filter", m_pos};
            }
            result.push_back(std::move(filterOpt.value()));
            
            skipWhitespace();
        }

        return result;
    }

private:
    std::string_view m_input;
    size_t m_pos = 0;
    size_t m_length = 0;

    void skipWhitespace() {
        while (m_pos < m_length && std::isspace(static_cast<unsigned char>(m_input[m_pos]))) {
            ++m_pos;
        }
    }

    bool consume(char c) {
        skipWhitespace();
        if (m_pos < m_length && m_input[m_pos] == c) {
            ++m_pos;
            return true;
        }
        return false;
    }

    std::optional<std::string_view> parseIdentifier() {
        skipWhitespace();
        size_t start = m_pos;
        while (m_pos < m_length && (std::isalnum(static_cast<unsigned char>(m_input[m_pos])) || m_input[m_pos] == '-')) {
            ++m_pos;
        }
        if (start == m_pos) return std::nullopt;
        return m_input.substr(start, m_pos - start);
    }

    std::optional<SkScalar> parseNumber() {
        skipWhitespace();
        size_t start = m_pos;
        
        // Handle optional sign
        if (m_pos < m_length && (m_input[m_pos] == '+' || m_input[m_pos] == '-')) {
            ++m_pos;
        }
        
        bool hasDigits = false;
        while (m_pos < m_length && std::isdigit(static_cast<unsigned char>(m_input[m_pos]))) {
            ++m_pos;
            hasDigits = true;
        }
        
        if (m_pos < m_length && m_input[m_pos] == '.') {
            ++m_pos;
            while (m_pos < m_length && std::isdigit(static_cast<unsigned char>(m_input[m_pos]))) {
                ++m_pos;
                hasDigits = true;
            }
        }
        
        if (!hasDigits) {
            m_pos = start;
            return std::nullopt;
        }
        
        std::string numStr(m_input.substr(start, m_pos - start));
        try {
            return static_cast<SkScalar>(std::stod(numStr));
        } catch (...) {
            m_pos = start;
            return std::nullopt;
        }
    }

    std::optional<SkColor> parseColor() {
        // Named colors (basic support)
        auto ident = parseIdentifier();
        if (ident) {
            return parseCssColor(std::string(*ident).c_str())->toSkColor();
        }
        
        return std::nullopt;
    }

    bool peek(std::string_view s) {
        if (m_pos + s.length() <= m_length) {
            return m_input.substr(m_pos, s.length()) == s;
        }
        return false;
    }


    std::optional<Filter> parseFilter() {
        auto identOpt = parseIdentifier();
        if (!identOpt) return std::nullopt;
        
        std::string_view ident = identOpt.value();
        
        if (!consume('(')) {
            return std::nullopt; // Expected opening paren
        }
        
        Filter filter;
        
        if (ident == "blur") {
            auto radius = parseLength();
            if (!radius) return std::nullopt;
            filter.type = Filter::Blur;
            filter.params = BlurParams{radius.value()*1.5f};
        }
        else if (ident == "brightness") {
            auto amount = parsePercentageOrNumber(1.0f);
            if (!amount) return std::nullopt;
            filter.type = Filter::Brightness;
            filter.params = BrightnessParams{amount.value()};
        }
        else if (ident == "contrast") {
            auto amount = parsePercentageOrNumber(1.0f);
            if (!amount) return std::nullopt;
            filter.type = Filter::Contrast;
            filter.params = ContrastParams{amount.value()};
        }
        else if (ident == "saturate") {
            auto amount = parsePercentageOrNumber(1.0f);
            if (!amount) return std::nullopt;
            filter.type = Filter::Saturate;
            filter.params = SaturateParams{amount.value()};
        }
        else if (ident == "grayscale") {
            auto amount = parsePercentageOrNumber(0.0f);
            if (!amount) return std::nullopt;
            filter.type = Filter::Grayscale;
            filter.params = GrayscaleParams{amount.value()};
        }
        else if (ident == "hue-rotate") {
            auto degrees = parseAngle();
            if (!degrees) return std::nullopt;
            filter.type = Filter::HueRotate;
            filter.params = HueRotateParams{degrees.value()};
        }
        else if (ident == "invert") {
            auto amount = parsePercentageOrNumber(0.0f);
            if (!amount) return std::nullopt;
            filter.type = Filter::Invert;
            filter.params = InvertParams{amount.value()};
        }
        else if (ident == "opacity") {
            auto amount = parsePercentageOrNumber(1.0f);
            if (!amount) return std::nullopt;
            filter.type = Filter::Opacity;
            filter.params = OpacityParams{amount.value()};
        }
        else if (ident == "sepia") {
            auto amount = parsePercentageOrNumber(0.0f);
            if (!amount) return std::nullopt;
            filter.type = Filter::Sepia;
            filter.params = SepiaParams{amount.value()};
        }
        else if (ident == "drop-shadow") {
            auto shadow = parseDropShadow();
            if (!shadow) return std::nullopt;
            filter.type = Filter::DropShadow;
            filter.params = shadow.value();
        }
        else {
            return std::nullopt; // Unknown filter
        }
        
        if (!consume(')')) {
            return std::nullopt; // Expected closing paren
        }
        
        return filter;
    }

    std::optional<SkScalar> parseLength() {
        auto num = parseNumber();
        if (!num) return std::nullopt;
        
        skipWhitespace();
        // Optional 'px' unit
        if (peek("px")) {
            m_pos += 2;
        }
        return num.value();
    }

    std::optional<float> parsePercentageOrNumber(float defaultValue) {
        auto num = parseNumber();
        if (!num) return std::nullopt;
        
        skipWhitespace();
        if (consume('%')) {
            return static_cast<float>(num.value() / 100.0);
        }
        return static_cast<float>(num.value());
    }

    std::optional<float> parseAngle() {
        auto num = parseNumber();
        if (!num) return std::nullopt;
        
        skipWhitespace();
        if (peek("deg")) {
            m_pos += 3;
        } else if (peek("rad")) {
            m_pos += 3;
            // Convert radians to degrees for storage
            return static_cast<float>(num.value() * 180.0 / M_PI);
        } else if (peek("turn")) {
            m_pos += 4;
            // Convert turns to degrees
            return static_cast<float>(num.value() * 360.0);
        }
        // Default is degrees
        return static_cast<float>(num.value());
    }

    std::optional<DropShadowParams> parseDropShadow() {
        DropShadowParams params;
        
        // dx
        auto dx = parseLength();
        if (!dx) return std::nullopt;
        params.dx = dx.value();
        
        // dy
        auto dy = parseLength();
        if (!dy) return std::nullopt;
        params.dy = dy.value();
        
        // blur-radius (optional, defaults to 0)
        skipWhitespace();
        size_t savePos = m_pos;
        auto blur = parseLength();
        if (blur) {
            params.blurRadius = blur.value();
        } else {
            m_pos = savePos;
        }
        
        // color (optional, defaults to black)
        skipWhitespace();
        auto color = parseColor();
        if (color) {
            params.color = color.value();
        }
        
        return params;
    }
};

// ============================================================================
// Skia Filter Builder
// ============================================================================

class SkiaFilterBuilder {
public:
    // Build a composed filter chain from parsed CSS filters
    // CSS filters apply left-to-right: filterA(filterB(filterC(input)))
    // Skia Compose(outer, inner) applies inner first, then outer
    static sk_sp<SkImageFilter> build(const std::vector<Filter>& filters) {
        sk_sp<SkImageFilter> result;
        
        for (const auto& filter : filters) {
            sk_sp<SkImageFilter> skFilter = createSkFilter(filter, result);
            if (skFilter) {
                result = skFilter;
            }
        }
        
        return result;
    }

    // Build with explicit crop rect
    static sk_sp<SkImageFilter> build(const std::vector<Filter>& filters, 
                                       const SkRect& cropRect) {
        sk_sp<SkImageFilter> result;
        
        for (const auto& filter : filters) {
            sk_sp<SkImageFilter> skFilter = createSkFilter(filter, result, &cropRect);
            if (skFilter) {
                result = skFilter;
            }
        }
        
        return result;
    }

private:
    static sk_sp<SkImageFilter> createSkFilter(const Filter& filter, 
                                                sk_sp<SkImageFilter> input,
                                                const SkRect* cropRect = nullptr) {
        switch (filter.type) {
            case Filter::Blur:
                return createBlurFilter(filter.blur(), std::move(input), cropRect);
            case Filter::Brightness:
                return createBrightnessFilter(filter.brightness(), std::move(input), cropRect);
            case Filter::Contrast:
                return createContrastFilter(filter.contrast(), std::move(input), cropRect);
            case Filter::Saturate:
                return createSaturateFilter(filter.saturate(), std::move(input), cropRect);
            case Filter::Grayscale:
                return createGrayscaleFilter(filter.grayscale(), std::move(input), cropRect);
            case Filter::HueRotate:
                return createHueRotateFilter(filter.hueRotate(), std::move(input), cropRect);
            case Filter::Invert:
                return createInvertFilter(filter.invert(), std::move(input), cropRect);
            case Filter::Opacity:
                return createOpacityFilter(filter.opacity(), std::move(input), cropRect);
            case Filter::Sepia:
                return createSepiaFilter(filter.sepia(), std::move(input), cropRect);
            case Filter::DropShadow:
                return createDropShadowFilter(filter.dropShadow(), std::move(input), cropRect);
            default:
                return input;
        }
    }

    static sk_sp<SkImageFilter> createBlurFilter(const BlurParams& params,
                                                  sk_sp<SkImageFilter> input,
                                                  const SkRect* cropRect) {
        // Skia uses sigma, CSS uses radius. 
        // CSS blur radius roughly maps to 2*sigma in Skia for similar visual effect
        SkScalar sigma = params.radius / 2;
        
        // IMPORTANT: For blur to not get "cut off", we need to ensure the filter
        // has enough space to sample. We use the default crop rect (empty/identity)
        // to let Skia auto-expand the bounds, OR we pass the explicit crop rect
        // but it must be expanded by the blur radius.
        if (cropRect) {
            // Expand crop rect by blur radius to prevent clipping
            SkRect expanded = *cropRect;
            expanded.outset(params.radius, params.radius);
            SkImageFilters::CropRect expandedCrop(expanded);
            return SkImageFilters::Blur(sigma, sigma, std::move(input), expandedCrop);
        }
        
        // Let Skia auto-calculate bounds (recommended for blur)
        return SkImageFilters::Blur(sigma, sigma, std::move(input));
    }

    static sk_sp<SkImageFilter> createBrightnessFilter(const BrightnessParams& params,
                                                        sk_sp<SkImageFilter> input,
                                                        const SkRect* cropRect) {
        SkColorMatrix matrix;
        matrix.setScale(params.amount, params.amount, params.amount, 1.0f);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createContrastFilter(const ContrastParams& params,
                                                    sk_sp<SkImageFilter> input,
                                                    const SkRect* cropRect) {
        float intercept = -0.5f * params.amount + 0.5f;
        float m[20] = {
            params.amount, 0, 0, 0, intercept,
            0, params.amount, 0, 0, intercept,
            0, 0, params.amount, 0, intercept,
            0, 0, 0, 1, 0
        };
        SkColorMatrix matrix;
        matrix.setRowMajor(m);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createSaturateFilter(const SaturateParams& params,
                                                      sk_sp<SkImageFilter> input,
                                                      const SkRect* cropRect) {
        float s = params.amount;
        float m[20] = {
            0.213f + 0.787f * s, 0.715f - 0.715f * s, 0.072f - 0.072f * s, 0, 0,
            0.213f - 0.213f * s, 0.715f + 0.285f * s, 0.072f - 0.072f * s, 0, 0,
            0.213f - 0.213f * s, 0.715f - 0.715f * s, 0.072f + 0.928f * s, 0, 0,
            0, 0, 0, 1, 0
        };
        SkColorMatrix matrix;
        matrix.setRowMajor(m);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createGrayscaleFilter(const GrayscaleParams& params,
                                                     sk_sp<SkImageFilter> input,
                                                     const SkRect* cropRect) {
        float g = params.amount;
        float s = 1.0f - g;
        float m[20] = {
            0.213f + 0.787f * s, 0.715f - 0.715f * s, 0.072f - 0.072f * s, 0, 0,
            0.213f - 0.213f * s, 0.715f + 0.285f * s, 0.072f - 0.072f * s, 0, 0,
            0.213f - 0.213f * s, 0.715f - 0.715f * s, 0.072f + 0.928f * s, 0, 0,
            0, 0, 0, 1, 0
        };
        SkColorMatrix matrix;
        matrix.setRowMajor(m);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createHueRotateFilter(const HueRotateParams& params,
                                                       sk_sp<SkImageFilter> input,
                                                       const SkRect* cropRect) {
        float cosVal = cosf(params.degrees * static_cast<float>(M_PI) / 180.0f);
        float sinVal = sinf(params.degrees * static_cast<float>(M_PI) / 180.0f);
        
        float m[20] = {
            0.213f + cosVal * 0.787f - sinVal * 0.213f,
            0.715f - cosVal * 0.715f - sinVal * 0.715f,
            0.072f - cosVal * 0.072f + sinVal * 0.928f,
            0, 0,
            
            0.213f - cosVal * 0.213f + sinVal * 0.143f,
            0.715f + cosVal * 0.285f + sinVal * 0.140f,
            0.072f - cosVal * 0.072f - sinVal * 0.283f,
            0, 0,
            
            0.213f - cosVal * 0.213f - sinVal * 0.787f,
            0.715f - cosVal * 0.715f + sinVal * 0.715f,
            0.072f + cosVal * 0.928f + sinVal * 0.072f,
            0, 0,
            
            0, 0, 0, 1, 0
        };
        SkColorMatrix matrix;
        matrix.setRowMajor(m);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createInvertFilter(const InvertParams& params,
                                                      sk_sp<SkImageFilter> input,
                                                      const SkRect* cropRect) {
        float a = params.amount;
        float m[20] = {
            1 - 2*a, 0, 0, 0, a,
            0, 1 - 2*a, 0, 0, a,
            0, 0, 1 - 2*a, 0, a,
            0, 0, 0, 1, 0
        };
        SkColorMatrix matrix;
        matrix.setRowMajor(m);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createOpacityFilter(const OpacityParams& params,
                                                     sk_sp<SkImageFilter> input,
                                                     const SkRect* cropRect) {
        SkColorMatrix matrix;
        matrix.setScale(1.0f, 1.0f, 1.0f, params.amount);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createSepiaFilter(const SepiaParams& params,
                                                   sk_sp<SkImageFilter> input,
                                                   const SkRect* cropRect) {
        float s = params.amount;
        float m[20] = {
            1 - 0.607f * s, 0.769f * s, 0.189f * s, 0, 0,
            0.349f * s, 1 - 0.314f * s, 0.168f * s, 0, 0,
            0.272f * s, 0.534f * s, 1 - 0.869f * s, 0, 0,
            0, 0, 0, 1, 0
        };
        SkColorMatrix matrix;
        matrix.setRowMajor(m);
        
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::ColorFilter(
                SkColorFilters::Matrix(matrix), std::move(input), c);
        }
        return SkImageFilters::ColorFilter(
            SkColorFilters::Matrix(matrix), std::move(input));
    }

    static sk_sp<SkImageFilter> createDropShadowFilter(const DropShadowParams& params,
                                                        sk_sp<SkImageFilter> input,
                                                        const SkRect* cropRect) {
        SkScalar sigmaX = params.blurRadius / 2;
        SkScalar sigmaY = params.blurRadius / 2;
        
        // DropShadow draws the shadow *behind* the content using the input as mask
        // The input content is preserved and drawn on top
        if (cropRect) {
            SkImageFilters::CropRect c(*cropRect);
            return SkImageFilters::DropShadow(
                params.dx, params.dy, 
                sigmaX, sigmaY, 
                params.color,
                std::move(input),
                c);
        }
        return SkImageFilters::DropShadow(
            params.dx, params.dy, 
            sigmaX, sigmaY, 
            params.color,
            std::move(input));
    }
};

//Internal API
inline std::variant<std::vector<Filter>, FilterParser::ParseError> 
parseFilters(std::string_view cssFilterString) {
    FilterParser parser;
    return parser.parse(cssFilterString);
}

inline sk_sp<SkImageFilter> buildSkiaFilter(std::string_view cssFilterString) {
    auto result = parseFilters(cssFilterString);
    if (std::holds_alternative<std::vector<Filter>>(result)) {
        return SkiaFilterBuilder::build(std::get<std::vector<Filter>>(result));
    }
    return nullptr;
}

} // namespace css