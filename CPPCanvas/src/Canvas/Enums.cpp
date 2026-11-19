enum class GradientType : int32_t {
    Linear,
    Radial,
    Conic
};

enum class CanvasTextBaseLine : int32_t {
    Top,
    Hanging,
    Middle,
    Alphabetic,
    Ideographic,
    Bottom,
};

enum class CanvasTextAlignment : int32_t {
    Left,
    Right,
    Center,
    Start,
    End
};

enum class CanvasSmoothingQuality : int32_t {
    Low,
    Medium,
    High,
};

enum class CanvasBatchOperationType : int32_t {
    // State & Style Setters
    SetFont = 1,
    SetFillStyleColor,
    SetFillStyleGradient,
    SetStrokeStyleColor,
    SetStrokeStyleGradient,
    SetLineWidth,
    SetGlobalAlpha,
    SetImageSmoothingQuality,
    SetImageSmoothingEnabled,
    SetFilter,
    SetGlobalCompositeOperation,
    SetShadowBlur,
    SetShadowColor,
    SetShadowOffsetX,
    SetShadowOffsetY,
    SetLineJoin,
    SetLineDash,
    SetLineDashOffset,
    SetTextBaseline,
    SetTextAlign,

    // State Stack & Transforms
    Save,
    Restore,
    Translate,
    Rotate,
    SetTransform,       // Accepts 6 floats (a, b, c, d, e, f) — DOMMatrix can be unpacked in JS
    Transform,
    ResetTransform,

    // Rectangles
    FillRect,
    StrokeRect,
    ClearRect,

    // Text
    FillText,
    StrokeText,

    // Paths
    BeginPath,
    MoveTo,
    LineTo,
    Arc,
    Ellipse,
    ArcTo,
    BezierCurveTo,
    QuadraticCurveTo,
    ClosePath,
    Rect,
    RoundRect,
    Fill,               // Uses current path
    FillPath,           // Uses explicit Path2D*
    Stroke,             // Uses current path
    StrokePath,         // Uses explicit Path2D*
    Clip,

    // Images & Pixel Buffers
    DrawImage_XYWH,
    DrawImage_SXYWH_DXYWH,
    PutImageData
};