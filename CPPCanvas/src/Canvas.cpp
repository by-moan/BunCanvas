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



// std::unordered_map<std::string,SkFilterMode> smoothingQualities{{
//     {"low",SkFilterMode::kLinear},
//     {"medium",SkFilterMode::kLinear},
//     {"high",SkFilterMode::kLinear}
// }};

class BunCanvas;

enum class GradientType {
    Linear,
    Radial,
    Conic
};

struct ColorStop {
    float offset;      // 0..1
    SkColor4f color;
};

class BunCanvasGradient {
    public:
    static constexpr uint64_t MAGIC = 0x48BB7416;
    uint64_t magic = MAGIC;
    
    GradientType type;
    
    // Linear
    SkPoint p0, p1;
    
    // Radial
    SkPoint c0, c1;
    float r0, r1;
    
    // Conic
    SkPoint center;
    float angle;
    
    std::vector<ColorStop> stops;
    
    BunCanvasGradient(GradientType gType, float x1, float y1, float x2, float y2) : type{gType}, p0{x1,y1}, p1{x2,y2} {}
};

typedef int (*JSCallback_CReset)();

class Rendering2DState {
    public:
    SkPathBuilder pathBuilder;
    SkPaint fillColor;
    sk_sp<SkShader> fillShader;
    SkPaint strokeColor;
    sk_sp<SkPathEffect> lineDashEffect;
    sk_sp<SkShader> strokeShader;
    sk_sp<SkImageFilter> filters;
    sk_sp<SkImageFilter> shadowBlurFilter;
    sk_sp<SkImageFilter> resolvedFilters;
    SkPaint imageColor;
    SkPaint shadowBlurColor;
    std::string cssFont = "10px sans-serif";
    SkFont font_current;
    float shadowBlurAmount = 0.f;
    float shadowBlurOffsetX = 0.f;
    float shadowBlurOffsetY = 0.f;
    int samplingQuality = 1;
    SkSamplingOptions sampling = SkFilterMode::kLinear;
    float globalAlpha = 1.f;

    Rendering2DState() = default;

    Rendering2DState(
        SkPath sPath,
        SkPaint fillColor,
        sk_sp<SkShader> fillShader,
        SkPaint strokeColor,
        sk_sp<SkPathEffect> lineDashEffect,
        sk_sp<SkShader> strokeShader,
        sk_sp<SkImageFilter> filters,
        sk_sp<SkImageFilter> shadowBlurFilter,
        sk_sp<SkImageFilter> resolvedFilters,
        SkPaint imageColor,
        SkPaint shadowBlurColor,
        std::string cssFont,
        CanvasFontCache font_cache,
        SkFont font_current,
        float shadowBlurAmount,
        float shadowBlurOffsetX,
        float shadowBlurOffsetY,
        SkSamplingOptions sampling
    ) : pathBuilder{sPath},
        fillColor{fillColor},
        fillShader{fillShader},
        strokeColor{strokeColor},
        lineDashEffect{lineDashEffect},
        strokeShader{strokeShader},
        filters{filters},
        shadowBlurFilter{shadowBlurFilter},
        resolvedFilters{resolvedFilters},
        imageColor{imageColor},
        shadowBlurColor{shadowBlurColor},
        cssFont{cssFont},
        font_current{font_current},
        shadowBlurAmount{shadowBlurAmount},
        shadowBlurOffsetX{shadowBlurOffsetX},
        shadowBlurOffsetY{shadowBlurOffsetY},
        sampling{SkFilterMode::kLinear}
        {}

    // Rendering2DState& operator=(const Rendering2DState& other) {
    //     if (this != &other) {
    //         pathBuilder = other.pathBuilder.snapshot();
    //         fillColor = other.fillColor;
    //         fillShader = other.fillShader;
    //         strokeColor = other.strokeColor;
    //         lineDashEffect = other.lineDashEffect;
    //         strokeShader = other.strokeShader;
    //         filters = other.filters;
    //         shadowBlurFilter = other.shadowBlurFilter;
    //         resolvedFilters = other.resolvedFilters;
    //         imageColor = other.imageColor;
    //         shadowBlurColor = other.shadowBlurColor;
    //         cssFont = other.cssFont;
    //         font_current = other.font_current;
    //         shadowBlurAmount = other.shadowBlurAmount;
    //         shadowBlurOffsetX = other.shadowBlurOffsetX;
    //         shadowBlurOffsetY = other.shadowBlurOffsetY;
    //         sampling = other.sampling;
    //     }
    //     return *this;
    // }
};

class BunCanvasRenderingContext2D {
    sk_sp<SkSurface> sfc = nullptr;
    public:
    BunCanvas* owner;
    CanvasFontCache font_cache;
    JSCallback_CReset jsResetCanvas;
    static constexpr uint64_t MAGIC = 0x5E5A8750;
    uint64_t magic = MAGIC;

    // SkPathBuilder pathBuilder;
    // SkPaint fillColor;
    // sk_sp<SkShader> fillShader;
    // SkPaint strokeColor;
    // sk_sp<SkPathEffect> lineDashEffect;
    // sk_sp<SkShader> strokeShader;
    // sk_sp<SkImageFilter> filters;
    // sk_sp<SkImageFilter> shadowBlurFilter;
    // sk_sp<SkImageFilter> resolvedFilters;
    // SkPaint imageColor;
    // SkPaint shadowBlurColor;
    // std::string cssFont;
    // SkFont font_current;
    // float shadowBlurAmount = 0.f;
    // float shadowBlurOffsetX = 0.f;
    // float shadowBlurOffsetY = 0.f;
    // SkSamplingOptions sampling;
    // int samplingQuality = 1;

    Rendering2DState currentState{};
    Rendering2DState savedState;
    
    
    BunCanvasRenderingContext2D(sk_sp<SkSurface> surface, BunCanvas* owner, JSCallback_CReset cb) : sfc(surface), jsResetCanvas(cb){
        this->owner = owner;
        currentState.strokeColor.setColor(SK_ColorBLACK);
        currentState.strokeColor.setStyle(SkPaint::kStroke_Style);
        currentState.strokeColor.setAlpha(255);
        currentState.strokeColor.setStrokeWidth(1);
        currentState.strokeColor.setAntiAlias(1);
        currentState.strokeColor.setBlendMode(compositeOperations.at("source-over"));
        currentState.strokeColor.setPathEffect(nullptr);
        currentState.strokeColor.setShader(nullptr);
        currentState.strokeColor.setStrokeCap(SkPaint::kButt_Cap);

        currentState.imageColor.setColor(SK_ColorWHITE);
        currentState.imageColor.setAlpha(255);
        currentState.imageColor.setStyle(SkPaint::kFill_Style);
        currentState.imageColor.setAntiAlias(1);
        currentState.imageColor.setBlendMode(compositeOperations.at("source-over"));
        
        currentState.fillColor.setColor(SK_ColorBLACK);
        currentState.fillColor.setStyle(SkPaint::kFill_Style);
        currentState.fillColor.setShader(nullptr);
        currentState.fillColor.setAlpha(255);
        currentState.fillColor.setAntiAlias(1);
        currentState.fillColor.setBlendMode(compositeOperations.at("source-over"));
        
        currentState.shadowBlurFilter = SkImageFilters::DropShadow(
            currentState.shadowBlurOffsetX,           // dx
            currentState.shadowBlurOffsetX,           // dy
            currentState.shadowBlurAmount * 0.5f, // sigmaX
            currentState.shadowBlurAmount * 0.5f, // sigmaY
            currentState.shadowBlurColor.getColor4f(),         // SkColor (uint32_t like SK_ColorBLACK or 0xFF000000)
            nullptr,                      // input (no input filter)
            nullptr                       // cropRect (optional, can omit)
        );
    }

    void save(){
        this->savedState = currentState;
        sfc->getCanvas()->save();
    }

    void restore(){
        currentState = savedState;
        sfc->getCanvas()->restore();
    }
    
    //Mimicking the behavior of values reset when resizing a canvas object.
    void reset(sk_sp<SkSurface> surface, sk_sp<SkImage> tmpImage) {
        // std::lock_guard<std::mutex> lock(draw_mutex);
        // ctx = surface->getCanvas();
        sfc = surface;
        sfc->getCanvas()->resetMatrix();
        currentState.pathBuilder.reset();
        currentState.fillColor.setBlendMode(compositeOperations.at("source-over"));
        currentState.fillColor.setColor(SK_ColorBLACK);
        currentState.fillColor.setStyle(SkPaint::kFill_Style);
        currentState.fillColor.setImageFilter(nullptr);
        currentState.fillColor.setAntiAlias(1);
        
        currentState.strokeColor.setBlendMode(compositeOperations.at("source-over"));
        currentState.strokeColor.setColor(SK_ColorBLACK);
        currentState.strokeColor.setStrokeJoin(SkPaint::Join::kMiter_Join);
        currentState.strokeColor.setStyle(SkPaint::kStroke_Style);
        currentState.strokeColor.setStrokeWidth(1);
        currentState.strokeColor.setImageFilter(nullptr);
        currentState.strokeColor.setAntiAlias(1);
        currentState.strokeColor.setStrokeCap(SkPaint::kDefault_Cap);
        
        currentState.shadowBlurAmount = 0.f;
        currentState.shadowBlurOffsetX = 0.f;
        currentState.shadowBlurOffsetY = 0.f;
        currentState.shadowBlurFilter = SkImageFilters::DropShadow(
            currentState.shadowBlurOffsetX,           // dx
            currentState.shadowBlurOffsetX,           // dy
            currentState.shadowBlurAmount * 0.5f, // sigmaX
            currentState.shadowBlurAmount * 0.5f, // sigmaY
            currentState.shadowBlurColor.getColor4f(),         // SkColor (uint32_t like SK_ColorBLACK or 0xFF000000)
            nullptr,                      // input (no input filter)
            nullptr                       // cropRect (optional, can omit)
        );
        currentState.shadowBlurColor.setImageFilter(currentState.shadowBlurFilter);

        currentState.samplingQuality = 1;
        currentState.sampling = SkFilterMode::kLinear;
        
        currentState.imageColor.setBlendMode(compositeOperations.at("source-over"));
        currentState.imageColor.setImageFilter(nullptr);

        
        sfc->getCanvas()->drawImage(tmpImage,0.f,0.f);
        
        currentState.cssFont = "10px sans-serif";

        currentState.filters = nullptr;
        
        font_cache.setFont(currentState.cssFont.c_str(),currentState.font_current);

        jsResetCanvas();
    }
    
    SkCanvas* operator()() {
        return sfc->getCanvas();
    }
};

enum CanvasContextType {
    None = 0,
    Rendering2D
};

struct RetiredTexture {
    GrBackendTexture tex;
    int framesLeft;
};

class BunCanvas {
    public:
    
    static constexpr uint64_t MAGIC = 0xBCA1155A;
    uint64_t magic = MAGIC;
    sk_sp<SkSurface> surface;
    std::mutex mutex;

    CanvasContextType ctxType;

    BunCanvasRenderingContext2D* rendering2D = nullptr;
    
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
    
    void* getContext2D(JSCallback_CReset cb) {
        if (!rendering2D) {
            ctxType = CanvasContextType::Rendering2D;
            rendering2D = new BunCanvasRenderingContext2D(surface,this,cb);
            return rendering2D;
        };
        return rendering2D;
    }
    
    void resize(int w, int h) {
        if (rendering2D == nullptr) return;
        // processRetiredTextures();
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
        auto tmp = surface->makeTemporaryImage();
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
        
        if (ctxType == CanvasContextType::Rendering2D) rendering2D->reset(surface, tmp);
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
    WINDOWS_EXPORT void* canvas_get_context2D(void* canvasObj, JSCallback_CReset cb){
        if (!canvasObj) return nullptr;
        BunCanvas* obj = validated<BunCanvas>(canvasObj);
        
        if (obj == nullptr) return nullptr;
        
        void* ptr = obj->getContext2D(cb);
        
        return ptr;
        
        // obj->ctx = obj->surface->getCanvas();
    }
    
    
    WINDOWS_EXPORT bool canvas_set_fill_style(void* renderingContext, const char* c){
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(renderingContext);
        if (obj == nullptr) return false;
        // nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        auto col = parseCssColor(c);
        if (col){
            obj->currentState.fillColor.setColor4f(*col);
            obj->currentState.fillColor.setShader(nullptr);
            return true;
        }
        return false;
    }
    
    WINDOWS_EXPORT bool canvas_set_fill_style_gradient(void* canvasObj, void* cGradient){
        
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        BunCanvasGradient* grad = validated<BunCanvasGradient>(cGradient);
        if (!obj||!grad) return false;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        
        switch (grad->type){
            case GradientType::Linear : {
                std::vector<SkColor4f> colors;
                std::vector<float> positions;
                
                colors.reserve(grad->stops.size());
                positions.reserve(grad->stops.size());
                
                for (const auto& stop : grad->stops) {
                    colors.push_back(stop.color);
                    positions.push_back(stop.offset);
                }
                
                SkGradient gradient(
                    SkGradient::Colors(colors, positions, SkTileMode::kClamp),
                    {}
                );
                SkPoint pts[] = { grad->p0, grad->p1 };
                obj->currentState.fillColor.setShader(SkShaders::LinearGradient(pts, gradient));
                return true;
            }
        }
        return false;
    }
    
    WINDOWS_EXPORT bool canvas_set_stroke_style(void* renderingContext, const char* c){
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(renderingContext);
        if (obj == nullptr) return false;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        
        auto col = parseCssColor(c);
        if (col){
            obj->currentState.strokeColor.setColor4f(*col);
            obj->currentState.strokeColor.setShader({});
            return true;
        }
        return false;
    }
    
    WINDOWS_EXPORT bool canvas_set_stroke_style_gradient(void* canvasObj, void* cGradient){
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        BunCanvasGradient* grad = validated<BunCanvasGradient>(cGradient);
        if (!obj||!grad) return false;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        
        switch (grad->type){
            case GradientType::Linear : {
                std::vector<SkColor4f> colors;
                std::vector<float> positions;
                
                colors.reserve(grad->stops.size());
                positions.reserve(grad->stops.size());
                
                for (const auto& stop : grad->stops) {
                    colors.push_back(stop.color);
                    positions.push_back(stop.offset);
                }
                
                SkGradient gradient(
                    SkGradient::Colors(colors, positions, SkTileMode::kClamp),
                    {}
                );
                SkPoint pts[] = { grad->p0, grad->p1 };
                obj->currentState.strokeColor.setShader(SkShaders::LinearGradient(pts, gradient));
                return true;
            }
        }
        return false;
    }
    WINDOWS_EXPORT bool canvas_set_shadow_color(void* renderingContext, const char* c){
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(renderingContext);
        if (obj == nullptr) return false;
        
        auto col = parseCssColor(c);
        if (col){
            obj->currentState.shadowBlurColor.setColor4f(*col);
            return true;
        }
        return false;
    }
    
    
    
    
    WINDOWS_EXPORT void canvas_set_stroke_width(void* canvasObj, float w) {
        if (!canvasObj) return;
        
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->currentState.strokeColor.setStrokeWidth(w);
    }
    
    WINDOWS_EXPORT void canvas_fill_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        // nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        // SkPaint fColor = obj->currentState.fillColor;
        // fColor.setAlphaf(obj->currentState.fillColor.getAlphaf()*obj->currentState.globalAlpha);
        // if(obj->currentState.shadowBlurAmount > 0.5f) {
        //     SkPaint shColor = obj->currentState.shadowBlurColor;
        //     auto _af = shColor.getAlphaf()*obj->currentState.globalAlpha;
        //     shColor.setAlphaf(_af);
        //     (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), shColor);
        // }
        (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), obj->currentState.fillColor);
    }
    
    WINDOWS_EXPORT void canvas_stroke_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        SkPaint sColor = obj->currentState.strokeColor;
        sColor.setAlphaf(obj->currentState.strokeColor.getAlphaf()*obj->currentState.globalAlpha);
        // if(obj->shadowBlurAmount > 0.5f) {
        //     SkPaint shColor = obj->shadowBlurColor;
        //     // shColor.setStyle(SkPaint::Style::kFill_Style);
        //     auto _af = shColor.getAlphaf()*obj->globalAlpha;
        //     shColor.setAlphaf(_af);
        //     // for (int i = 0 ; i < 10 ; i++) (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), shColor);
        //     sk_sp<SkSurface> sfc = SkSurfaces::Raster(
        //         SkImageInfo::MakeN32Premul(w,h)
        //     );
        //     sfc->getCanvas()->drawRect(SkRect::MakeXYWH(0,0,w,h),obj->strokeColor);
        //     for (int i = 0 ; i < 2 ; i++) (*obj)()->drawImageRect(sfc->makeTemporaryImage(),SkRect::MakeXYWH(x,y,w,h), obj->sampling, &shColor);
        //     // shColor.setStyle(SkPaint::Style::kFill_Style);
        // }
        (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), sColor);
    }
    
    WINDOWS_EXPORT void canvas_clear_rect(void* canvasObj, int x, int y, int w, int h) {
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        // if (!obj) return;
        // std::cout << "(*obj)() = " << (*obj)() << "\n";
        // std::cout << "obj = " << obj << "\n";
        // std::cout << "obj->owner->magic == BunCanvas::MAGIC = " << (obj->owner->magic == BunCanvas::MAGIC) << "\n";
        // std::cout << "obj->magic == BunCanvasRenderingContext2d::MAGIC = " << (obj->magic == BunCanvasRenderingContext2D::MAGIC) << "\n";
        
        if (!obj) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), clearColor);
    }
    
    WINDOWS_EXPORT void canvas_resize(void* canvasObj, int w, int h) {
        if (!canvasObj) return;
        BunCanvas* obj = validated<BunCanvas>(canvasObj);
        
        if (obj == nullptr) {
            return;
        };
        
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
        obj->currentState.pathBuilder.reset();
    }
    
    WINDOWS_EXPORT void canvas_path_move_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->currentState.pathBuilder.moveTo(x,y);
    }
    WINDOWS_EXPORT void canvas_path_line_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->currentState.pathBuilder.lineTo(x,y);
    }
    
    WINDOWS_EXPORT void canvas_path_arc(void* canvasObj, float x1, float y1, float radius, float startAngle, float sweepangle) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->currentState.pathBuilder.addArc(SkRect::MakeXYWH(x1 - radius,y1 - radius,radius * 2,radius * 2), startAngle*57.29577958f, sweepangle*57.29577958f);
    }
    
    WINDOWS_EXPORT void canvas_path_arc_to(void* canvasObj, float x1, float y1, float x2, float y2, float radius) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->currentState.pathBuilder.arcTo({x1,y1},{x2,y2},radius);
    }
    
    WINDOWS_EXPORT void canvas_path_bezier_to(void* canvasObj, float x1, float y1, float x2, float y2, float x3, float y3) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->currentState.pathBuilder.cubicTo(x1,y1,x2,y2,x3,y3);
    }
    WINDOWS_EXPORT void canvas_path_fill(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) {
            return;
        };
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        
        SkPaint fColor = obj->currentState.fillColor;
        auto alphaf = obj->currentState.fillColor.getAlphaf()*obj->currentState.globalAlpha;
        fColor.setAlphaf(alphaf);
        (*obj)()->drawPath(obj->currentState.pathBuilder.snapshot(), fColor);
    }
    WINDOWS_EXPORT void canvas_path_stroke(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) {
            return;
        };
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        
        SkPaint sColor = obj->currentState.strokeColor;
        auto alphaf = obj->currentState.strokeColor.getAlphaf()*obj->currentState.globalAlpha;
        sColor.setAlphaf(alphaf);
        
        // if(obj->shadowBlurAmount > 0.5f) {
        //     SkPaint shColor = obj->shadowBlurColor;
        //     // shColor.setStyle(SkPaint::Style::kFill_Style);
        //     auto _af = shColor.getAlphaf()*obj->globalAlpha;
        //     shColor.setAlphaf(_af);
        //     // for (int i = 0 ; i < 10 ; i++) (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), shColor);
        //     int w = obj->owner->surface->width();
        //     int h = obj->owner->surface->height();
        //     sk_sp<SkSurface> sfc = SkSurfaces::Raster(
        //         SkImageInfo::MakeN32Premul(w,h)
        //     );
        //     // sfc->getCanvas()->drawRect(SkRect::MakeXYWH(0,0,w,h),obj->strokeColor);
        //     sfc->getCanvas()->drawPath(obj->pathBuilder.snapshot(), sColor);
        //     for (int i = 0 ; i < 2 ; i++) (*obj)()->drawImageRect(sfc->makeTemporaryImage(),SkRect::MakeXYWH(0,0,w,h), obj->sampling, &shColor);
        // }
        (*obj)()->drawPath(obj->currentState.pathBuilder.snapshot(), sColor);
    }
    
    WINDOWS_EXPORT void canvas_path_close(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->currentState.pathBuilder.close();
    }
    
    WINDOWS_EXPORT void canvas_draw_image_imageType(void* canvasObj, void* image, float x,float y,float w,float h) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        ImageWrapper* img = static_cast<ImageWrapper*>(image);
        if (obj == nullptr || img == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        // if(obj->shadowBlurAmount > 0.5f) {
        //     SkPaint shColor = obj->shadowBlurColor;
        //     auto _af = shColor.getAlphaf()*obj->globalAlpha;
        //     shColor.setAlphaf(_af);
        //     SkMatrix savedMatrix = (*obj)()->getTotalMatrix();
        //     auto dst = SkRect::MakeXYWH(x,y,w,h);
        //     (*obj)()->saveLayer(dst,&(shColor));
        //     (*obj)()->drawImageRect(img->image.get(),dst,obj->sampling,&(shColor));
        //     (*obj)()->restore();
        //     (*obj)()->setMatrix(savedMatrix);
        // }
        (*obj)()->drawImageRect(img->image.get(),SkRect::MakeXYWH(x,y,w,h),obj->currentState.sampling,&(obj->currentState.imageColor));
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
                // if(obj->shadowBlurAmount > 0.5f) {
                //     SkPaint shColor = obj->shadowBlurColor;
                //     auto _af = shColor.getAlphaf()*obj->globalAlpha;
                //     shColor.setAlphaf(_af);
                //     (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(shColor));
                // }
                (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->currentState.sampling,&(obj->currentState.imageColor));
            }
        } else {
            auto _img = img->surface->makeTemporaryImage();
            // if(obj->shadowBlurAmount > 0.5f) {
            //     SkPaint shColor = obj->shadowBlurColor;
            //     auto _af = shColor.getAlphaf()*obj->globalAlpha;
            //     shColor.setAlphaf(_af);
            //     (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(shColor));
            // }
            (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->currentState.sampling,&(obj->currentState.imageColor));
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
            obj->currentState.fillColor.setBlendMode(compositeOperations.at(name));
            obj->currentState.strokeColor.setBlendMode(compositeOperations.at(name));
            obj->currentState.imageColor.setBlendMode(compositeOperations.at(name));
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
            
            (*obj)()->drawImage(bitmap.asImage(),x,y,obj->currentState.sampling,&pImageDataColor);
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
        obj->currentState.globalAlpha = _a;
        obj->currentState.imageColor.setAlphaf(_a);
        return _a;
    }
    WINDOWS_EXPORT void canvas_save(void* canvasObj){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->save();
    }
    WINDOWS_EXPORT void canvas_restore(void* canvasObj){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(std::lock_guard<std::mutex> lock(obj->owner->mutex));
        obj->restore();
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
        (*obj)()->rotate(deg*57.29577951308232);
    }
    
    WINDOWS_EXPORT bool canvas_set_font(void* ctxPtr, const char* cssString) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        return ctx->font_cache.setFont(cssString,ctx->currentState.font_current);
    }
    
    WINDOWS_EXPORT void canvas_fill_text(void* ctxPtr, const char* txt, float x, float y, float maxWidth) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        float width = ctx->currentState.font_current.measureText(
            txt,
            std::strlen(txt),
            SkTextEncoding::kUTF8
        );
        SkPaint fColor = ctx->currentState.fillColor;
        auto alphaf = fColor.getAlphaf()*ctx->currentState.globalAlpha;
        fColor.setAlphaf(alphaf);
        if (width > maxWidth && maxWidth > -1) {
            float scale = maxWidth / width;
            (*ctx)()->save();
            (*ctx)()->translate(x, y);
            (*ctx)()->scale(scale, 1.0f);
            if(ctx->currentState.shadowBlurAmount > 0.5f) {
                SkPaint shColor = ctx->currentState.shadowBlurColor;
                auto _af = shColor.getAlphaf()*ctx->currentState.globalAlpha;
                shColor.setAlphaf(_af);
                (*ctx)()->drawString(txt, x+ctx->currentState.shadowBlurOffsetX, y+ctx->currentState.shadowBlurOffsetY, ctx->currentState.font_current, shColor);
            }
            (*ctx)()->drawString(txt, x, y, ctx->currentState.font_current, fColor);
            (*ctx)()->restore();
        } else {
            (*ctx)()->drawString(txt, x, y, ctx->currentState.font_current, fColor);
        }
    }
    WINDOWS_EXPORT void canvas_stroke_text(void* ctxPtr, const char* txt, float x, float y, float maxWidth) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        float width = ctx->currentState.font_current.measureText(
            txt,
            std::strlen(txt),
            SkTextEncoding::kUTF8
        );
        SkPaint sColor = ctx->currentState.strokeColor;
        sColor.setAlphaf(sColor.getAlphaf()*ctx->currentState.globalAlpha);
        if (width > maxWidth && maxWidth > -1) {
            float scale = maxWidth / width;
            (*ctx)()->save();
            (*ctx)()->translate(x, y);
            (*ctx)()->scale(scale, 1.0f);
            if(ctx->currentState.shadowBlurAmount > 0.5f) {
                SkPaint shColor = ctx->currentState.shadowBlurColor;
                auto _af = shColor.getAlphaf()*ctx->currentState.globalAlpha;
                shColor.setAlphaf(_af);
                (*ctx)()->drawString(txt, x+ctx->currentState.shadowBlurOffsetX, y+ctx->currentState.shadowBlurOffsetY, ctx->currentState.font_current, shColor);
            }
            (*ctx)()->drawString(txt, x, y, ctx->currentState.font_current, sColor);
            (*ctx)()->restore();
        } else {
            (*ctx)()->drawString(txt, x, y, ctx->currentState.font_current, sColor);
        }
    }
    
    WINDOWS_EXPORT void canvas_set_shadow_blur(void* ctxPtr, float b){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        ctx->currentState.shadowBlurAmount = std::max(b,0.f);
        
        if (ctx->currentState.shadowBlurAmount > 0.2f){
            ctx->currentState.shadowBlurFilter = SkImageFilters::DropShadow(
                ctx->currentState.shadowBlurOffsetX,           // dx
                ctx->currentState.shadowBlurOffsetY,           // dy
                ctx->currentState.shadowBlurAmount * 0.5f, // sigmaX
                ctx->currentState.shadowBlurAmount * 0.5f, // sigmaY
                ctx->currentState.shadowBlurColor.getColor4f(),         // SkColor (uint32_t like SK_ColorBLACK or 0xFF000000)
                nullptr,                      // input (no input filter)
                nullptr                       // cropRect (optional, can omit)
            );
        }else {
            ctx->currentState.shadowBlurFilter = nullptr;
        }
        if (ctx->currentState.filters){
            ctx->currentState.resolvedFilters = SkImageFilters::Compose(ctx->currentState.shadowBlurFilter, ctx->currentState.filters);
        }else{
            ctx->currentState.resolvedFilters = ctx->currentState.shadowBlurFilter;
        }
        // ctx->currentState.fillColor.setImageFilter(ctx->currentState.resolvedFilters);
        ctx->currentState.imageColor.setImageFilter(ctx->currentState.resolvedFilters);
        ctx->currentState.strokeColor.setImageFilter(ctx->currentState.resolvedFilters);
        ctx->currentState.shadowBlurColor.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, ctx->currentState.shadowBlurAmount * 0.5f));
    }
    WINDOWS_EXPORT void canvas_set_shadow_offsetX(void* ctxPtr, float oX){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        
        ctx->currentState.shadowBlurOffsetX = oX;
    }
    
    WINDOWS_EXPORT void canvas_set_shadow_offsetY(void* ctxPtr, float oY){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        
        ctx->currentState.shadowBlurOffsetY = oY;
    }

    WINDOWS_EXPORT bool canvas_set_image_smoothing_enabled(void* ctxPtr, bool v){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        
        if (v){
            switch(ctx->currentState.samplingQuality) {
                case 1:{
                    ctx->currentState.sampling = SkFilterMode::kLinear;
                    return true;
                }
                case 2: {
                    ctx->currentState.sampling = SkFilterMode::kLinear;
                    return true;
                }
                case 3: {
                    ctx->currentState.sampling = SkCubicResampler::Mitchell();
                    return true;
                }
            }
        }else{
            ctx->currentState.samplingQuality = 0;
            ctx->currentState.sampling = SkFilterMode::kNearest;
            return true;
        }

        return false;
    }

    WINDOWS_EXPORT bool canvas_set_image_smoothing_quality(void* ctxPtr, int v){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        switch(v) {
            case 1:{
                ctx->currentState.sampling = SkFilterMode::kLinear;
                ctx->currentState.samplingQuality = v;
                return true;
            }
            case 2: {
                ctx->currentState.sampling = SkFilterMode::kLinear;
                ctx->currentState.samplingQuality = v;
                return true;
            }
            case 3: {
                ctx->currentState.sampling = SkCubicResampler::Mitchell();
                std::cout << v << "\n";
                ctx->currentState.samplingQuality = v;
                return true;
            }
        }
        return false;
    }
    
    WINDOWS_EXPORT void* canvas_create_linear_gradient(float x1, float y1, float x2, float y2){
        return new BunCanvasGradient(GradientType::Linear, x1, y1, x2, y2);
    }
    WINDOWS_EXPORT bool canvas_gradient_add_color_stop(void* gPtr, float offset, const char* c) {
        // Validate offset (0 <= offset <= 1)
        // Throw IndexSizeError if invalid
        auto* grad = validated<BunCanvasGradient>(gPtr);
        if (!grad) return false;
        
        auto it = std::lower_bound(
            grad->stops.begin(),
            grad->stops.end(),
            offset,
            [](const ColorStop& s, float o) {
                return s.offset < o;
            });
            
            auto col = parseCssColor(c);
            
            if (col) {
                grad->stops.insert(it, {offset, *col});
                return true;
            }
            return false;
    }
    WINDOWS_EXPORT void canvas_gradient_destroy(void* gPtr) {
        // Validate offset (0 <= offset <= 1)
        // Throw IndexSizeError if invalid
        auto* grad = validated<BunCanvasGradient>(gPtr);
        if (!grad) return;
        delete grad;
    }
    
    WINDOWS_EXPORT void canvas_destroy(void* ptr) {
        auto* cnv = validated<BunCanvas>(ptr);
        if (!cnv) return;
        
        auto it = std::find(canvases.begin(), canvases.end(), ptr);
        
        if (it != canvases.end()) {
            canvases.erase(it);
        }
        delete cnv;
    }
    WINDOWS_EXPORT bool canvas_get_transform(void* ctxPtr, float* mtx){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        
        auto matrix = (*ctx)()->getLocalToDevice();
        matrix.getColMajor(mtx);
        return mtx[0]  == 1 && mtx[1]  == 0 && mtx[2]  == 0 && mtx[3]  == 0 &&
        mtx[4]  == 0 && mtx[5]  == 1 && mtx[6]  == 0 && mtx[7]  == 0 &&
        mtx[8]  == 0 && mtx[9]  == 0 && mtx[10] == 1 && mtx[11] == 0 &&
        mtx[12] == 0 && mtx[13] == 0 && mtx[14] == 0 && mtx[15] == 1;

        // SkImageFilters::Compose
    }
    WINDOWS_EXPORT bool canvas_set_filter(void* ctxPtr, const char* fltr){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        auto filter = css::buildSkiaFilter(fltr);
        if (std::strcmp(fltr, "none") == 0){
            ctx->currentState.filters = nullptr;
            if (ctx->currentState.shadowBlurFilter){
                ctx->currentState.resolvedFilters = ctx->currentState.shadowBlurFilter;
            }else{
                ctx->currentState.resolvedFilters = nullptr;
            }
            ctx->currentState.fillColor.setImageFilter(ctx->currentState.resolvedFilters);
            ctx->currentState.imageColor.setImageFilter(ctx->currentState.resolvedFilters);
            ctx->currentState.strokeColor.setImageFilter(ctx->currentState.resolvedFilters);
            return true;
        }
        if (filter) {
            // SkPaint paint;
            ctx->currentState.filters = filter;
            if (ctx->currentState.shadowBlurFilter){
                ctx->currentState.resolvedFilters = SkImageFilters::Compose(ctx->currentState.shadowBlurFilter, ctx->currentState.filters);
            }else{
                ctx->currentState.resolvedFilters = filter;
            }
            ctx->currentState.fillColor.setImageFilter(ctx->currentState.resolvedFilters);
            ctx->currentState.imageColor.setImageFilter(ctx->currentState.resolvedFilters);
            ctx->currentState.strokeColor.setImageFilter(ctx->currentState.resolvedFilters);
            return true;
        }
        return false;
    }

    WINDOWS_EXPORT void canvas_set_line_dash(void* ctxPtr, float* pts, int len, float offset){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));

        if(!pts || len <= 0) ctx->currentState.strokeColor.setPathEffect(nullptr);
        ctx->currentState.lineDashEffect = SkDashPathEffect::Make(SkSpan<const SkScalar>(pts, len),offset);
        ctx->currentState.strokeColor.setPathEffect(ctx->currentState.lineDashEffect);
    }

    WINDOWS_EXPORT bool canvas_set_line_join(void* ctxPtr, int lJoinType){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(std::lock_guard<std::mutex> lock(ctx->owner->mutex));
        switch(lJoinType) {
            case 1:{
                ctx->currentState.strokeColor.setStrokeJoin(SkPaint::Join::kRound_Join);
                return true;
            }
            case 2: {
                ctx->currentState.strokeColor.setStrokeJoin(SkPaint::Join::kBevel_Join);
                return true;
            }
            case 3: {
                ctx->currentState.strokeColor.setStrokeJoin(SkPaint::Join::kMiter_Join);
                return true;
            }
        }
        return false;
    }
}