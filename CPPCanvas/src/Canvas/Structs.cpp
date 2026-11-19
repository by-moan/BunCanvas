namespace CanvasBatchCommands {
    struct SetFont {
        CanvasBatchOperationType op;
        int32_t _pad;
        const char* cssString;
    };
    static_assert(sizeof(SetFont) == 16);

    struct SetFillStyle {
        CanvasBatchOperationType op;
        int32_t _pad;
        const char* color;
    };
    static_assert(sizeof(SetFillStyle) == 16);

    struct SetFillStyleGradient {
        CanvasBatchOperationType op;
        int32_t gradType; // GradientType
        float* gradientArgs;
        int32_t stopsLength;
        int32_t _pad;
        const char** stopsColor;
        float* stopsOffset;
    };
    static_assert(sizeof(SetFillStyleGradient) == 40);

    struct SetStrokeStyle {
        CanvasBatchOperationType op;
        int32_t _pad;
        const char* color;
    };
    static_assert(sizeof(SetStrokeStyle) == 16);

    struct SetStrokeStyleGradient {
        CanvasBatchOperationType op;
        int32_t gradType; // GradientType
        float* gradientArgs;
        int32_t stopsLength;
        int32_t _pad;
        const char** stopsColor;
        float* stopsOffset;
    };
    static_assert(sizeof(SetStrokeStyleGradient) == 40);

    struct SetLineWidth {
        CanvasBatchOperationType op;
        int32_t _pad;
        double width;
    };
    static_assert(sizeof(SetLineWidth) == 16);

    struct SetGlobalAlpha {
        CanvasBatchOperationType op;
        int32_t _pad;
        double alpha;
    };
    static_assert(sizeof(SetGlobalAlpha) == 16);

    struct SetImageSmoothingQuality {
        CanvasBatchOperationType op;
        int32_t quality; // CanvasSmoothingQuality
    };
    static_assert(sizeof(SetImageSmoothingQuality) == 8);

    struct SetImageSmoothingEnabled {
        CanvasBatchOperationType op;
        int32_t enabled; // bool as int32_t (0 or 1)
    };
    static_assert(sizeof(SetImageSmoothingEnabled) == 8);

    struct SetFilter {
        CanvasBatchOperationType op;
        int32_t _pad;
        const char* filter;
    };
    static_assert(sizeof(SetFilter) == 16);

    struct SetGlobalCompositeOperation {
        CanvasBatchOperationType op;
        int32_t blendMode; // SkBlendMode
    };
    static_assert(sizeof(SetGlobalCompositeOperation) == 8);

    struct SetShadowBlur {
        CanvasBatchOperationType op;
        int32_t _pad;
        double blur;
    };
    static_assert(sizeof(SetShadowBlur) == 16);

    struct SetShadowColor {
        CanvasBatchOperationType op;
        int32_t _pad;
        const char* color;
    };
    static_assert(sizeof(SetShadowColor) == 16);

    struct SetShadowOffsetX {
        CanvasBatchOperationType op;
        int32_t _pad;
        double offsetX;
    };
    static_assert(sizeof(SetShadowOffsetX) == 16);

    struct SetShadowOffsetY {
        CanvasBatchOperationType op;
        int32_t _pad;
        double offsetY;
    };
    static_assert(sizeof(SetShadowOffsetY) == 16);

    struct SetLineJoin {
        CanvasBatchOperationType op;
        int32_t joinType; // SkPaint::Join
    };
    static_assert(sizeof(SetLineJoin) == 8);

    struct SetLineDash {
        CanvasBatchOperationType op;
        int32_t len;
        float* pts;
        double offset;
    };
    static_assert(sizeof(SetLineDash) == 24);

    struct SetTextBaseline {
        CanvasBatchOperationType op;
        int32_t baseline; // CanvasTextBaseLine
    };
    static_assert(sizeof(SetTextBaseline) == 8);

    struct SetTextAlign {
        CanvasBatchOperationType op;
        int32_t alignment; // CanvasTextAlignment
    };
    static_assert(sizeof(SetTextAlign) == 8);

    #pragma region State Stack & Transforms

    struct Save {
        CanvasBatchOperationType op;
        int32_t _pad;
    };
    static_assert(sizeof(Save) == 8);

    struct Restore {
        CanvasBatchOperationType op;
        int32_t _pad;
    };
    static_assert(sizeof(Restore) == 8);

    struct Translate {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
    };
    static_assert(sizeof(Translate) == 24);

    struct Rotate {
        CanvasBatchOperationType op;
        int32_t _pad;
        double radians;
    };
    static_assert(sizeof(Rotate) == 16);

    struct SetTransform {
        CanvasBatchOperationType op;
        int32_t _pad;
        double mtx[16];
    };
    static_assert(sizeof(SetTransform) == 136);

    struct ResetTransform {
        CanvasBatchOperationType op;
        int32_t _pad;
    };
    static_assert(sizeof(ResetTransform) == 8);

    #pragma endregion

    #pragma region Rectangles

    struct FillRect {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
        double w;
        double h;
    };
    static_assert(sizeof(FillRect) == 40);

    struct StrokeRect {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
        double w;
        double h;
    };
    static_assert(sizeof(StrokeRect) == 40);

    struct ClearRect {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
        double w;
        double h;
    };
    static_assert(sizeof(ClearRect) == 40);

    #pragma endregion

    #pragma region Text

    struct FillText {
        CanvasBatchOperationType op;
        int32_t _pad;
        const char* text;
        double x;
        double y;
        double maxWidth;
    };
    static_assert(sizeof(FillText) == 40);

    struct StrokeText {
        CanvasBatchOperationType op;
        int32_t _pad;
        const char* text;
        double x;
        double y;
        double maxWidth;
    };
    static_assert(sizeof(StrokeText) == 40);

    #pragma region Paths

    struct BeginPath {
        CanvasBatchOperationType op;
        int32_t _pad;
    };
    static_assert(sizeof(BeginPath) == 8);

    struct MoveTo {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
    };
    static_assert(sizeof(MoveTo) == 24);

    struct LineTo {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
    };
    static_assert(sizeof(LineTo) == 24);

    struct Arc {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
        double radius;
        double startAngle;
        double sweepAngle;
    };
    static_assert(sizeof(Arc) == 48);

    struct Ellipse {
        CanvasBatchOperationType op;
        int32_t counterclockwise; // bool as int32_t (0 or 1)
        double x;
        double y;
        double radiusX;
        double radiusY;
        double rotation;
        double startAngle;
        double endAngle;
    };
    static_assert(sizeof(Ellipse) == 64);

    struct ArcTo {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x1;
        double y1;
        double x2;
        double y2;
        double radius;
    };
    static_assert(sizeof(ArcTo) == 48);

    struct BezierCurveTo {
        CanvasBatchOperationType op;
        int32_t _pad;
        double cp1x;
        double cp1y;
        double cp2x;
        double cp2y;
        double x;
        double y;
    };
    static_assert(sizeof(BezierCurveTo) == 56);

    struct QuadraticCurveTo {
        CanvasBatchOperationType op;
        int32_t _pad;
        double cpx;
        double cpy;
        double x;
        double y;
    };
    static_assert(sizeof(QuadraticCurveTo) == 40);

    struct ClosePath {
        CanvasBatchOperationType op;
        int32_t _pad;
    };
    static_assert(sizeof(ClosePath) == 8);

    struct Rect {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
        double w;
        double h;
    };
    static_assert(sizeof(Rect) == 40);

    struct RoundRect {
        CanvasBatchOperationType op;
        int32_t _pad;
        double x;
        double y;
        double w;
        double h;
        double radius;
    };
    static_assert(sizeof(RoundRect) == 48);

    struct RoundRectArray {
        CanvasBatchOperationType op;
        int32_t len;
        double x;
        double y;
        double w;
        double h;
        float* radii;
    };
    static_assert(sizeof(RoundRectArray) == 48);

    struct FillPath {
        CanvasBatchOperationType op;
        int32_t _pad;
        void* path2d; // Nullable Path2D*
    };
    static_assert(sizeof(FillPath) == 16);

    struct StrokePath {
        CanvasBatchOperationType op;
        int32_t _pad;
        void* path2d; // Nullable Path2D*
    };
    static_assert(sizeof(StrokePath) == 16);

    struct Clip {
        CanvasBatchOperationType op;
        int32_t _pad;
    };
    static_assert(sizeof(Clip) == 8);

    #pragma endregion

    #pragma region Images & Pixel Buffers

    struct DrawImage_ImageType {
        CanvasBatchOperationType op;
        int32_t argc;
        void* image;       // ImageWrapper*
        float* dimensions;
    };
    static_assert(sizeof(DrawImage_ImageType) == 24);

    struct DrawImage_CanvasType {
        CanvasBatchOperationType op;
        int32_t argc;
        void* canvas;      // BunCanvas*
        float* dimensions;
    };
    static_assert(sizeof(DrawImage_CanvasType) == 24);

    struct PutImageData {
        CanvasBatchOperationType op;
        int32_t dx;
        int32_t dy;
        int32_t imageWidth;
        int32_t imageHeight;
        int32_t dirtyX;
        int32_t dirtyY;
        int32_t dirtyWidth;
        int32_t dirtyHeight;
        int32_t _pad;
        uint8_t* buffer;
    };
    static_assert(sizeof(PutImageData) == 48);
};