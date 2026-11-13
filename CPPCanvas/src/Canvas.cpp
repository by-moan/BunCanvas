SkPaint g_clearColor;
SkPaint g_noAlphaClearColor;
SkPaint pImageDataColor;

class BunCanvas;

enum class GradientType {
    Linear,
    Radial,
    Conic
};

enum class CanvasTextBaseLine {
    Top,
    Hanging,
    Middle,
    Alphabetic,
    Ideographic,
    Bottom,
};

enum class CanvasTextAlignment {
    Left,
    Right,
    Center,
    Start,
    End
};

enum class CanvasSmoothingQuality {
    Low,
    Medium,
    High,
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
    
    BunCanvasGradient(GradientType gType, float x1, float y1, float a) : type{gType}, center{x1,y1}, angle{a} {}
    BunCanvasGradient(GradientType gType, float x1, float y1, float x2, float y2) : type{gType}, p0{x1,y1}, p1{x2,y2} {}
    BunCanvasGradient(GradientType gType, float x1, float y1, float x2, float y2, float _r0, float _r1) : type{gType}, c0{x1,y1}, c1{x2,y2}, r0{_r0}, r1{_r1} {}
};

typedef int (*JSCallback_CReset)();

class Rendering2DState {
    public:
    Path2D path;
    CanvasTextBaseLine textBaseline = CanvasTextBaseLine::Alphabetic;
    CanvasTextAlignment textAlign = CanvasTextAlignment::Left;
    SkPaint fillColor;
    sk_sp<SkShader> fillShader;
    SkPaint strokeColor;
    sk_sp<SkPathEffect> lineDashEffect;
    sk_sp<SkShader> strokeShader;
    sk_sp<SkImageFilter> filters{nullptr};
    sk_sp<SkImageFilter> shadowBlurFilter{nullptr};
    sk_sp<SkImageFilter> resolvedFilters{nullptr};
    SkPaint imageColor;
    SkPaint shadowBlurColor;
    // std::string cssFont = "10px sans-serif";
    SkFont font_current;
    float shadowBlurAmount = 0.f;
    float shadowBlurOffsetX = 0.f;
    float shadowBlurOffsetY = 0.f;
    CanvasSmoothingQuality samplingQuality = CanvasSmoothingQuality::Low;
    SkSamplingOptions sampling = SkFilterMode::kLinear;
    float globalAlpha = 1.f;

    Rendering2DState() {
        path.reset();
        fillColor.setBlendMode(SkBlendMode::kSrcOver);
        fillColor.setColor(SK_ColorBLACK);
        fillColor.setStyle(SkPaint::kFill_Style);
        fillColor.setImageFilter(nullptr);
        fillColor.setAntiAlias(1);
        
        strokeColor.setBlendMode(SkBlendMode::kSrcOver);
        strokeColor.setColor(SK_ColorBLACK);
        strokeColor.setStrokeJoin(SkPaint::Join::kMiter_Join);
        strokeColor.setStyle(SkPaint::kStroke_Style);
        strokeColor.setStrokeWidth(1);
        strokeColor.setImageFilter(nullptr);
        strokeColor.setAntiAlias(1);
        strokeColor.setStrokeCap(SkPaint::kDefault_Cap);
        shadowBlurColor.setColor4f({0,0,0,0});
        shadowBlurColor.setImageFilter(shadowBlurFilter);
        sampling = SkFilterMode::kLinear;
        
        imageColor.setBlendMode(SkBlendMode::kSrcOver);
        imageColor.setImageFilter(nullptr);
    };
};

class BunCanvasRenderingContext2D {
    sk_sp<SkSurface> sfc = nullptr;
    public:
    BunCanvas* owner;
    CanvasFontCache font_cache;
    JSCallback_CReset jsResetCanvas;
    static constexpr uint64_t MAGIC = 0x5E5A8750;
    uint64_t magic = MAGIC;
    bool desynchronized = false;
    bool alpha = true;
    SkPaint clearColor;

    Rendering2DState currentState{};
    Rendering2DState savedState;
    
    
    BunCanvasRenderingContext2D(
        sk_sp<SkSurface> surface,
        BunCanvas* owner,
        JSCallback_CReset cb,
        bool desync,
        bool alpha
    ) : sfc(surface), owner{owner}, jsResetCanvas(cb), desynchronized(desync), alpha(alpha), clearColor(alpha?g_clearColor:g_noAlphaClearColor) {}

    void save(){
        this->savedState = currentState;
        sfc->getCanvas()->save();
    }

    void restore(){
        currentState = savedState;
        sfc->getCanvas()->restore();
    }
    
    //Mimicking the behavior of values reset when resizing a canvas object.
    void reset(sk_sp<SkSurface> surface, SkImage* tmpImage) {
     sfc = surface;
     sfc->getCanvas()->resetMatrix();
     currentState = Rendering2DState{};
     savedState = Rendering2DState{};

     jsResetCanvas();

     if (tmpImage) {
         sfc->getCanvas()->drawImageRect(
             tmpImage,
             SkRect::MakeXYWH(0, 0, tmpImage->width(), tmpImage->height()),
             currentState.sampling
         );
     }

     font_cache.setFont("10px sans-serif", currentState.font_current);
 }

    void reset(sk_sp<SkSurface> surface, sk_sp<SkImage> tmpImage) {
        sfc = surface;
        sfc->getCanvas()->resetMatrix();
        currentState = Rendering2DState{};
        savedState = Rendering2DState{};

        if (tmpImage) {
            sfc->getCanvas()->drawImageRect(
                tmpImage,
                SkRect::MakeXYWH(0, 0, tmpImage->width(), tmpImage->height()),
                currentState.sampling
            );
        }
        jsResetCanvas();

        font_cache.setFont("10px sans-serif", currentState.font_current);
        tmpImage.reset();
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
    bool useGPU = true;
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
    
    
    BunCanvas(int w, int h, bool gpu = true) : useGPU(gpu){
        // nonapple(std::lock_guard lock(mutex, std::defer_lock));
        #ifdef __APPLE__
        if (renderThreadContext == nullptr){
            surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w,h));
        }else {
            surface = SkSurfaces::RenderTarget(renderThreadContext->context.get(), skgpu::Budgeted::kYes, SkImageInfo::MakeN32Premul(w,h));
        }
        #else
        if (gpuContextReady && useGPU) {
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
        if (mainThreadCtx && mainThreadCtx->context) {
            for (auto& r : retiredTextures) {
                mainThreadCtx->context->deleteBackendTexture(r.tex);
            }
        }
        retiredTextures.clear();
        #endif
    }
    
    void* getContext2D(JSCallback_CReset cb, bool desync, bool alpha) {
        if (!rendering2D) {
            ctxType = CanvasContextType::Rendering2D;
            rendering2D = new BunCanvasRenderingContext2D(surface,this,cb, desync, alpha);
            return rendering2D;
        };
        return rendering2D;
    }
    
    void resize(int w, int h) {
        if (!rendering2D) return;
        sk_sp<SkImage> tmp;

        #ifdef __APPLE__
        if (surface) {
            tmp = surface->makeImageSnapshot();
        }
        surface.reset();
        if (renderThreadContext == nullptr) {
            surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w, h));
        } else {
            surface = SkSurfaces::RenderTarget(
                renderThreadContext->context.get(),
                skgpu::Budgeted::kYes,
                SkImageInfo::MakeN32Premul(w, h)
            );
        }
        if (surface) {
            surface->getCanvas()->clear(SK_ColorTRANSPARENT);
        }
        #else
        if (gpuContextReady && useGPU) {
            GrBackendTexture oldTex = backendTex;
            bool hadOldTex = hasBackendTex;

            backendTex = mainThreadCtx->context->createBackendTexture(
                w, h,
                kN32_SkColorType,
                skgpu::Mipmapped::kNo,
                GrRenderable::kYes,
                GrProtected::kNo
            );

            if (!retiredTextures.empty() && mainThreadCtx && mainThreadCtx->context) {
                for (auto it = retiredTextures.begin(); it != retiredTextures.end(); ) {
                    it->framesLeft--;
                    if (it->framesLeft <= 0) {
                        mainThreadCtx->context->deleteBackendTexture(it->tex);
                        it = retiredTextures.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            if (backendTex.isValid()) {
                surface.reset();
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

                // Clear entire new texture so expanded borders never expose uninitialized VMA memory
                surface->getCanvas()->clear(SK_ColorTRANSPARENT);

                // GPU-to-GPU copy from old texture without CPU readback
                if (hadOldTex && oldTex.isValid()) {
                    tmp = SkImages::BorrowTextureFrom(
                        mainThreadCtx->context.get(),
                        oldTex,
                        kTopLeft_GrSurfaceOrigin,
                        kN32_SkColorType,
                        kPremul_SkAlphaType,
                        nullptr
                    );
                }

                if (ctxType == CanvasContextType::Rendering2D) {
                    rendering2D->reset(surface, std::move(tmp));
                }

                // Transition new backendTex to SHADER_READ_ONLY_OPTIMAL before releasing element->mutex
                skgpu::MutableTextureState readState =
                    skgpu::MutableTextureStates::MakeVulkan(
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        VK_QUEUE_FAMILY_IGNORED
                    );
                mainThreadCtx->context->flush(surface.get(), GrFlushInfo{}, &readState);
                mainThreadCtx->context->flushAndSubmit(GrSyncCpu::kYes);

                if (hadOldTex) {
                    retiredTextures.push_back({oldTex, 3});
                }
                return;
            } else {
                tmp = surface ? surface->makeTemporaryImage() : nullptr;
                surface.reset();
                surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w, h));
                surface->getCanvas()->clear(SK_ColorTRANSPARENT);
                hasBackendTex = false;
            }
        } else {
            tmp = surface ? surface->makeTemporaryImage() : nullptr;
            surface.reset();
            surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w, h));
            surface->getCanvas()->clear(SK_ColorTRANSPARENT);
            hasBackendTex = false;
        }
        #endif

        if (ctxType == CanvasContextType::Rendering2D) {
            rendering2D->reset(surface, std::move(tmp));
        }
    }
};


std::vector<BunCanvas*> canvases;

void VK::flushAndSyncMainContext(GrDirectContext* mainCtx) {
    if (!mainCtx) return;;

    skgpu::MutableTextureState readState =
        skgpu::MutableTextureStates::MakeVulkan(
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_QUEUE_FAMILY_IGNORED
        );

    for (auto* element : canvases) {
        std::lock_guard<std::mutex> lock(element->mutex);
        #ifdef __APPLE__
        if (element->surface) {
            mainCtx->flush(element->surface.get(), GrFlushInfo{}, &readState);
        }
        #else
        if (element->hasBackendTex && element->surface) {
            mainCtx->flush(element->surface.get(), GrFlushInfo{}, &readState);
        }
        #endif
        
    }

    std::unique_lock<std::mutex> lock(sharedQueueMtx, std::defer_lock);
    if (mainQueue == renderQueue) lock.lock();
    mainCtx->flushAndSubmit(GrSyncCpu::kYes);
}

void drawImage(BunCanvasRenderingContext2D* ctx, sk_sp<SkImage> img, float* d, int argc){
    if (argc == 4) {
        (*ctx)()->drawImageRect(img.get(),SkRect::MakeXYWH(d[0], d[1], d[2], d[3]),ctx->currentState.sampling,&(ctx->currentState.imageColor));
    }
    if (argc == 8) {
        (*ctx)()->drawImageRect(
            img.get(),
            SkRect::MakeXYWH(d[0],d[1],d[2],d[3]),
            SkRect::MakeXYWH(d[4],d[5],d[6],d[7]),
            ctx->currentState.sampling,
            &(ctx->currentState.imageColor),
            {}
        );
    }
}

static sk_sp<SkShader> buildSkiaShader(
    GradientType gradType, 
    const float* args, 
    int stops_length, 
    const char** stops_color, 
    const float* stops_offset
) {
    if (stops_length <= 0) return nullptr;

    std::vector<SkColor4f> colors;
    std::vector<float> positions;
    colors.reserve(stops_length);
    positions.reserve(stops_length);

    for (int i = 0; i < stops_length; i++) {
        auto col = parseCssColor(stops_color[i]);
        if (col) {
            colors.push_back(*col);
            positions.push_back(stops_offset[i]);
        }
    }

    if (colors.empty()) return nullptr;

    SkGradient gradient(SkGradient::Colors(colors, positions, SkTileMode::kClamp), {});

    switch (gradType) {
        case GradientType::Linear: {
            SkPoint pts[] = { {args[0], args[1]}, {args[2], args[3]} };
            return SkShaders::LinearGradient(pts, gradient);
        }
        case GradientType::Radial: {
            SkPoint c0{args[0], args[1]}, c1{args[2], args[3]};
            float r0 = args[4], r1 = args[5];
            if (c0 == c1 && r0 == 0.0f) {
                return SkShaders::RadialGradient(c1, r1, gradient);
            }
            return SkShaders::TwoPointConicalGradient(c0, r0, c1, r1, gradient);
        }
        case GradientType::Conic: {
            SkPoint center{args[0], args[1]};
            float startDeg = SkRadiansToDegrees(args[2]);
            return SkShaders::SweepGradient(center, startDeg, startDeg + 360.0f, gradient, nullptr);
        }
    }
    return nullptr;
}

// Internal dispatcher to handle locking and state assignment
static bool setCanvasGradientInternal(
    void* canvasObj, 
    GradientType gradType,
    const float* gradient_args,
    int stops_length, 
    const char** stops_color, 
    const float* stops_offset,
    bool isStroke
) {
    BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
    if (!obj || !gradient_args || !stops_color || !stops_offset) return false;

    nonapple(
        std::optional<std::lock_guard<std::mutex>> lock;
        if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
    );

    sk_sp<SkShader> shader = buildSkiaShader(gradType, gradient_args, stops_length, stops_color, stops_offset);
    if (!shader) return false;

    if (isStroke) {
        obj->currentState.strokeColor.setShader(std::move(shader));
    } else {
        obj->currentState.fillColor.setShader(std::move(shader));
    }

    return true;
}

void updateCanvasFilters(Rendering2DState& state) {
    // 1. Rebuild shadow filter from current parameters
    if (state.shadowBlurAmount > 0.2f) {
        state.shadowBlurFilter = SkImageFilters::DropShadow(
            state.shadowBlurOffsetX,
            state.shadowBlurOffsetY,
            state.shadowBlurAmount * 0.5f,
            state.shadowBlurAmount * 0.5f,
            state.shadowBlurColor.getColor4f(),
            nullptr,
            nullptr
        );
    } else {
        state.shadowBlurFilter = nullptr;
    }

    // 2. Resolve filter pipeline
    if (state.shadowBlurFilter && state.filters) {
        state.resolvedFilters = SkImageFilters::Compose(state.shadowBlurFilter, state.filters);
    } else if (state.shadowBlurFilter) {
        state.resolvedFilters = state.shadowBlurFilter;
    } else {
        state.resolvedFilters = state.filters;
    }

    // 3. Bind resolved filter to all paint states
    state.fillColor.setImageFilter(state.resolvedFilters);
    state.imageColor.setImageFilter(state.resolvedFilters);
    state.strokeColor.setImageFilter(state.resolvedFilters);
}

extern "C" {
    WINDOWS_EXPORT void* canvas_create(int w, int h, bool useGPU) {
        return new BunCanvas(w,h,useGPU);
    }
    
    WINDOWS_EXPORT void canvas_append(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvas* obj = static_cast<BunCanvas*>(canvasObj);
        if (obj->magic != BunCanvas::MAGIC)
        return;
        
        canvases.push_back(obj);
    }
    
    //Sets native internal context so it is returned if is asked again.
    WINDOWS_EXPORT void* canvas_get_context2D(void* canvasObj, JSCallback_CReset cb, bool desync, bool alpha){
        if (!canvasObj) return nullptr;
        BunCanvas* obj = validated<BunCanvas>(canvasObj);
        
        if (obj == nullptr) return nullptr;
        
        void* ptr = obj->getContext2D(cb, desync, alpha);
        
        return ptr;
        
        // obj->ctx = obj->surface->getCanvas();
    }
    
    
    WINDOWS_EXPORT bool canvas_set_fill_style(void* renderingContext, const char* c){
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(renderingContext);
        if (obj == nullptr) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        auto col = parseCssColor(c);
        if (col){
            obj->currentState.fillColor.setColor4f(*col);
            obj->currentState.fillColor.setShader(nullptr);
            return true;
        }
        return false;
    }
    
    WINDOWS_EXPORT bool canvas_set_stroke_style_gradient(
        void* canvasObj, GradientType gradType, float* gradient_args,
        int stops_length, const char** stops_color, float* stops_offset
    ) {
        return setCanvasGradientInternal(canvasObj, gradType, gradient_args, stops_length, stops_color, stops_offset, true);
    }
    WINDOWS_EXPORT bool canvas_set_stroke_style(void* renderingContext, const char* c){
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(renderingContext);
        if (obj == nullptr) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        
        auto col = parseCssColor(c);
        if (col){
            obj->currentState.strokeColor.setColor4f(*col);
            obj->currentState.strokeColor.setShader({});
            return true;
        }
        return false;
    }
    
    WINDOWS_EXPORT bool canvas_set_fill_style_gradient(
        void* canvasObj, GradientType gradType, float* gradient_args,
        int stops_length, const char** stops_color, float* stops_offset
    ) {
        return setCanvasGradientInternal(canvasObj, gradType, gradient_args, stops_length, stops_color, stops_offset, false);
    }
    
    
    WINDOWS_EXPORT void canvas_set_stroke_width(void* canvasObj, float w) {
        if (!canvasObj) return;
        
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.strokeColor.setStrokeWidth(w);
    }
    
    WINDOWS_EXPORT void canvas_fill_rect(void* canvasObj, int x, int y, int w, int h) {
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        
        SkPaint fColor = obj->currentState.fillColor;
        fColor.setAlphaf(obj->currentState.fillColor.getAlphaf()*obj->currentState.globalAlpha);
        if(obj->currentState.shadowBlurAmount > 0.01f) {
            SkPaint shColor = obj->currentState.shadowBlurColor;
            auto _af = shColor.getAlphaf()*obj->currentState.globalAlpha;
            shColor.setAlphaf(_af);
            (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), shColor);
        }
        (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), fColor);
    }
    
    WINDOWS_EXPORT void canvas_stroke_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
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
        
        if (!obj) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        (*obj)()->drawRect(SkRect::MakeXYWH(x,y,w,h), obj->clearColor);
    }
    
    WINDOWS_EXPORT void canvas_resize(void* canvasObj, int w, int h) {
        if (!canvasObj) return;
        BunCanvas* obj = validated<BunCanvas>(canvasObj);
        if (obj == nullptr || std::isnan(w) || std::isnan(h)) {
            return;
        };
        {
            //This should lock at all costs. Otherwise it will cause a segfault
            //because the surface becomes invalid for a brief period
            nonapple(std::lock_guard<std::mutex> lock(obj->mutex));
            obj->resize(std::clamp(w, 1, 2147483647),std::clamp(h, 1, 2147483647));
        }
    }
    
    WINDOWS_EXPORT void canvas_path_begin(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.reset();
    }
    
    WINDOWS_EXPORT void canvas_path_move_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.moveTo(x,y);
    }
    WINDOWS_EXPORT void canvas_path_line_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.lineTo(x,y);
    }
    
    WINDOWS_EXPORT void canvas_path_arc(void* canvasObj, float x1, float y1, float radius, float startAngle, float sweepangle) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.arc(x1,y1,radius,startAngle,sweepangle);
    }

    WINDOWS_EXPORT void canvas_path_ellipse(
        void* canvasObj,
        float x,
        float y,
        float radiusX,
        float radiusY,
        float rotation,
        float startAngle,
        float endAngle,
        bool counterclockwise
    )   {
        if (radiusX < 0 || radiusY < 0) {
            return;
        }
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;

        obj->currentState.path.ellipse(x,y,radiusX,radiusY,rotation,startAngle,endAngle,counterclockwise);
    }
    
    WINDOWS_EXPORT void canvas_path_arc_to(void* canvasObj, float x1, float y1, float x2, float y2, float radius) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.arcTo(x1,y1,x2,y2,radius);
    }
    
    WINDOWS_EXPORT void canvas_path_bezier_to(void* canvasObj, float x1, float y1, float x2, float y2, float x3, float y3) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.bezierTo(x1,y1,x2,y2,x3,y3);
    }
    WINDOWS_EXPORT void canvas_path_quad_to(void* canvasObj, float x1, float y1, float x2, float y2) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.quadraticCurveTo(x1,y1,x2,y2);
    }
    WINDOWS_EXPORT void canvas_path_fill(void* canvasObj, void* path2d) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        Path2D* path = validated<Path2D>(path2d);
        
        if (obj == nullptr) {
            return;
        };
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        
        SkPath finalPath = path?(*path)():obj->currentState.path();
        SkPaint fColor = obj->currentState.fillColor;
        fColor.setAlphaf(obj->currentState.fillColor.getAlphaf()*obj->currentState.globalAlpha);
        if(obj->currentState.shadowBlurColor.getAlpha() > 0) {
            SkPaint shColor = obj->currentState.shadowBlurColor;
            auto _af = shColor.getAlphaf()*obj->currentState.globalAlpha;
            shColor.setAlphaf(_af);
            (*obj)()->drawPath(finalPath, shColor);
        }
        (*obj)()->drawPath(finalPath, fColor);
    }
    WINDOWS_EXPORT void canvas_path_stroke(void* canvasObj, void* path2d) {
        if (!canvasObj) return;

        BunCanvasRenderingContext2D* obj =
            validated<BunCanvasRenderingContext2D>(canvasObj);

        Path2D* path = validated<Path2D>(path2d);

        if (!obj) return;

        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized)
                lock.emplace(obj->owner->mutex);
        );

        SkPath finalPath =
            path ? (*path)() : obj->currentState.path();


        if (obj->currentState.shadowBlurColor.getAlpha() > 0) {

            SkPaint shadowPaint = obj->currentState.shadowBlurColor;

            float alpha =
                obj->currentState.shadowBlurColor.getAlphaf() *
                obj->currentState.globalAlpha;

            (*obj)()->drawPath(finalPath, shadowPaint);
        }

        SkPaint strokePaint = obj->currentState.strokeColor;

        strokePaint.setAlphaf(
            obj->currentState.strokeColor.getAlphaf() *
            obj->currentState.globalAlpha
        );

        (*obj)()->drawPath(finalPath, strokePaint);
    }
    WINDOWS_EXPORT void canvas_path_clip(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) {
            return;
        };
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        (*obj)()->clipPath(obj->currentState.path());
    }
    
    WINDOWS_EXPORT void canvas_path_close(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.close();
    }

    WINDOWS_EXPORT void canvas_path_rect(void* canvasObj, float x, float y, float w, float h) {
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.rect(x,y,w,h);
    }
    WINDOWS_EXPORT void canvas_path_roundrect(void* canvasObj, float x, float y, float w, float h, float radii) {
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.roundRect(x,y,w,h,radii);
    }
    WINDOWS_EXPORT void canvas_path_roundrect_array(void* canvasObj, float x, float y, float w, float h, float* radii, int len) {
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.path.roundRect_array(x,y,w,h,radii,len);
    }
    
    WINDOWS_EXPORT void canvas_draw_image_imageType(void* canvasObj, void* image, float* dimensions, int argc) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        ImageWrapper* img = static_cast<ImageWrapper*>(image);
        if (obj == nullptr || img == nullptr || !dimensions) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
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
        drawImage(obj,img->image,dimensions,argc);
        // if (argc == 4) {
            // (*obj)()->drawImageRect(img->image.get(),SkRect::MakeXYWH(x,y,w,h),obj->currentState.sampling,&(obj->currentState.imageColor));
        // }
    }
    
    WINDOWS_EXPORT void canvas_draw_image_canvasType(void* canvasObj, void* image, float* dimensions, int argc) {
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        BunCanvas* img = validated<BunCanvas>(image);
        if (obj == nullptr || img == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock2;
            if (img->rendering2D && !img->rendering2D->desynchronized) lock2.emplace(img->mutex);
        );
        #ifndef __APPLE__
        if (img->hasBackendTex) {
            auto _img = SkImages::BorrowTextureFrom(
                mainThreadCtx->context.get(),
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
                drawImage(obj,_img,dimensions,argc);
            }
        } else {
            auto _img = img->surface->makeTemporaryImage();
            // if(obj->shadowBlurAmount > 0.5f) {
            //     SkPaint shColor = obj->shadowBlurColor;
            //     auto _af = shColor.getAlphaf()*obj->globalAlpha;
            //     shColor.setAlphaf(_af);
            //     (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->sampling,&(shColor));
            // }
            // (*obj)()->drawImageRect(_img,SkRect::MakeXYWH(x,y,w,h),obj->currentState.sampling,&(obj->currentState.imageColor));
            drawImage(obj,_img,dimensions,argc);
        }
        #else
        auto _img = img->surface->makeTemporaryImage();
        drawImage(obj,_img,dimensions,argc);
        #endif
    }
    
    WINDOWS_EXPORT bool canvas_set_composite_operation(void* canvasObj, SkBlendMode operation) {
        if (!canvasObj) return false;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.fillColor.setBlendMode(operation);
        obj->currentState.strokeColor.setBlendMode(operation);
        obj->currentState.imageColor.setBlendMode(operation);
        return true;
    }
    
    WINDOWS_EXPORT bool canvas_get_image_data(void* canvasObj,int x, int y, int w, int h, uint8_t* out_buffer) {
        if (!canvasObj) return false;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        if (obj == nullptr) {
            return false;
        };
        {
            nonapple(
                std::optional<std::lock_guard<std::mutex>> lock;
                if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
            );
            SkImageInfo dstInfo = SkImageInfo::Make(
                w,h,
                kRGBA_8888_SkColorType,
                kPremul_SkAlphaType
            );
            size_t rowBytes = w * 4;
            
            #ifndef __APPLE__
            if (gpuContextReady && obj->owner->hasBackendTex) {
                auto img = SkImages::BorrowTextureFrom(
                    mainThreadCtx->context.get(),
                    obj->owner->backendTex,
                    kTopLeft_GrSurfaceOrigin,
                    kN32_SkColorType,
                    kPremul_SkAlphaType,
                    nullptr
                );
                
                if (img) {
                    // canvas->drawImage(img, 0, 0);
                    return img->readPixels(
                        dstInfo, 
                        out_buffer, 
                        rowBytes, 
                        x, 
                        y, 
                        SkImage::CachingHint::kDisallow_CachingHint
                    );
                }else {
                    return false;
                }
            }
            #else
            auto img = obj->owner->surface->makeImageSnapshot();
                
                if (img) {
                    // canvas->drawImage(img, 0, 0);
                    return img->readPixels(
                        dstInfo, 
                        out_buffer, 
                        rowBytes, 
                        x, 
                        y, 
                        SkImage::CachingHint::kDisallow_CachingHint
                    );
                }else {
                    return false;
                }
            #endif
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
    
    
    WINDOWS_EXPORT bool canvas_put_image_data(
        void* canvasObj,
        int dx,
        int dy,
        int imageWidth,
        int imageHeight,
        int dirtyX,
        int dirtyY,
        int dirtyWidth,
        int dirtyHeight,
        uint8_t* buffer
    ) {
        if (!canvasObj || !buffer)
            return false;

        BunCanvasRenderingContext2D* obj =
            validated<BunCanvasRenderingContext2D>(canvasObj);

        if (obj == nullptr)
            return false;

        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized)
                lock.emplace(obj->owner->mutex);
        );

        // Normalize negative dirty dimensions.
        if (dirtyWidth < 0) {
            dirtyX += dirtyWidth;
            dirtyWidth = -dirtyWidth;
        }

        if (dirtyHeight < 0) {
            dirtyY += dirtyHeight;
            dirtyHeight = -dirtyHeight;
        }

        // Empty rectangle.
        if (dirtyWidth == 0 || dirtyHeight == 0)
            return true;

        // Clip dirty rectangle against ImageData.
        int srcX = std::max(dirtyX, 0);
        int srcY = std::max(dirtyY, 0);

        int srcRight = std::min(
            dirtyX + dirtyWidth,
            imageWidth
        );

        int srcBottom = std::min(
            dirtyY + dirtyHeight,
            imageHeight
        );

        if (srcX >= srcRight || srcY >= srcBottom)
            return true;

        int width = srcRight - srcX;
        int height = srcBottom - srcY;

        // Destination must move along with the clipped source rectangle.
        int destX = dx + (srcX - dirtyX);
        int destY = dy + (srcY - dirtyY);

        // Create an SkBitmap representing the selected region.
        SkImageInfo imageInfo = SkImageInfo::Make(
            width,
            height,
            kRGBA_8888_SkColorType,
            kUnpremul_SkAlphaType
        );

        SkBitmap bitmap;

        // Point into the original ImageData buffer.
        uint8_t* src = buffer +
            (static_cast<size_t>(srcY) * imageWidth + srcX) * 4;

        if (!bitmap.installPixels(
            imageInfo,
            src,
            static_cast<size_t>(imageWidth) * 4
        )) {
            return false;
        }

        SkCanvas* canvas = (*obj)();

        if (!canvas)
            return false;

        SkPaint paint;
        paint.setBlendMode(SkBlendMode::kSrc);

        canvas->drawImage(
            bitmap.asImage(),
            destX,
            destY,
            obj->currentState.sampling,
            &paint
        );

        return true;
    }
    WINDOWS_EXPORT float canvas_set_global_alpha(void* canvasObj, float a){
        float _a = std::clamp(a, 0.0f, 1.0f);
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        if (obj == nullptr) return _a;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->currentState.globalAlpha = _a;
        obj->currentState.imageColor.setAlphaf(_a);
        obj->currentState.fillColor.setAlphaf(_a);
        obj->currentState.strokeColor.setAlphaf(_a);
        obj->currentState.shadowBlurColor.setAlphaf(_a);
        return _a;
    }
    WINDOWS_EXPORT void canvas_save(void* canvasObj){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->save();
    }
    WINDOWS_EXPORT void canvas_restore(void* canvasObj){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        obj->restore();
    }
    WINDOWS_EXPORT void canvas_translate(void* canvasObj, float x, float y){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        (*obj)()->translate(x,y);
    }
    WINDOWS_EXPORT void canvas_rotate(void* canvasObj, float deg){
        if (!canvasObj) return;
        BunCanvasRenderingContext2D* obj = validated<BunCanvasRenderingContext2D>(canvasObj);
        
        if (obj == nullptr) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!obj->desynchronized) lock.emplace(obj->owner->mutex);
        );
        (*obj)()->rotate(deg*57.29577951308232);
    }
    
    WINDOWS_EXPORT bool canvas_set_font(void* ctxPtr, const char* cssString) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        return ctx->font_cache.setFont(cssString,ctx->currentState.font_current);
    }
    
    WINDOWS_EXPORT void canvas_fill_text(void* ctxPtr, const char* txt, float x, float y, float maxWidth) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        
        float width = ctx->currentState.font_current.measureText(
            txt,
            std::strlen(txt),
            SkTextEncoding::kUTF8
        );
        
        SkPaint fColor = ctx->currentState.fillColor;
        auto alphaf = fColor.getAlphaf() * ctx->currentState.globalAlpha;
        fColor.setAlphaf(alphaf);

        SkFontMetrics m;
        ctx->currentState.font_current.getMetrics(&m);
        float finalY = 0.f;
        switch (ctx->currentState.textBaseline) {
            case CanvasTextBaseLine::Top :
                finalY = y - m.fTop;
                break;

            case CanvasTextBaseLine::Middle:
                finalY = y - (m.fAscent + m.fDescent) * 0.5f;
                break;

            case CanvasTextBaseLine::Alphabetic:
                finalY = y;
                break;

            case CanvasTextBaseLine::Bottom:
                finalY = y - m.fBottom;
                break;

            case CanvasTextBaseLine::Hanging:
                finalY = y - m.fAscent;
                break;

            case CanvasTextBaseLine::Ideographic:
                finalY = y - m.fDescent;
                break;
            default: break;
        }
        float finalX = 0.f;
        switch (ctx->currentState.textAlign) {
            case CanvasTextAlignment::Left :
                finalX = x;
                break;
            case CanvasTextAlignment::Right :
                finalX = x - width;
                break;
            case CanvasTextAlignment::Center :
                finalX = x - width/2;
                break;
            case CanvasTextAlignment::Start :
                finalX = x;
                break;
            case CanvasTextAlignment::End :
                finalX = x - width;
                break;
            default: break;
        }
        
        if (width > maxWidth && maxWidth > -1) {
            float scale = maxWidth / width;
            (*ctx)()->save();
            (*ctx)()->translate(x, y);
            (*ctx)()->scale(scale, 1.0f);
            // if(ctx->currentState.shadowBlurColor.getAlpha() > 0) {
            //     SkPaint shColor = ctx->currentState.shadowBlurColor;
            //     auto _af = shColor.getAlphaf()*ctx->currentState.globalAlpha;
            //     shColor.setAlphaf(_af);
            //     (*ctx)()->drawString(txt, finalX+ctx->currentState.shadowBlurOffsetX, finalY+ctx->currentState.shadowBlurOffsetY, ctx->currentState.font_current, shColor);
            // }
            (*ctx)()->drawString(txt, finalX, finalY, ctx->currentState.font_current, fColor);
            (*ctx)()->restore();
        } else {
            (*ctx)()->drawString(txt, finalX, finalY, ctx->currentState.font_current, fColor);
        }

        // (*ctx)()->drawString(txt, finalX, finalY, ctx->currentState.font_current, fColor);
    }

    WINDOWS_EXPORT void canvas_stroke_text(void* ctxPtr, const char* txt, float x, float y, float maxWidth) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
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
            if(ctx->currentState.shadowBlurColor.getAlpha() > 0) {
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
    
    WINDOWS_EXPORT bool canvas_set_filter(void* ctxPtr, const char* fltr) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;

        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );

        if (std::strcmp(fltr, "none") == 0) {
            ctx->currentState.filters = nullptr;
            updateCanvasFilters(ctx->currentState);
            return true;
        }

        auto filter = css::buildSkiaFilter(fltr);
        if (!filter) return false;

        ctx->currentState.filters = std::move(filter);
        updateCanvasFilters(ctx->currentState);
        return true;
    }

    WINDOWS_EXPORT void canvas_set_shadow_blur(void* ctxPtr, float b) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;

        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );

        ctx->currentState.shadowBlurAmount = std::max(b, 0.0f);
        

        updateCanvasFilters(ctx->currentState);
    }

    WINDOWS_EXPORT void canvas_set_shadow_offsetX(void* ctxPtr, float oX) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;

        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );

        if (ctx->currentState.shadowBlurOffsetX == oX) return;
        ctx->currentState.shadowBlurOffsetX = oX;
        
        // Rebuild filters only if a active shadow exists
        if (ctx->currentState.shadowBlurAmount > 0.2f) {
            updateCanvasFilters(ctx->currentState);
        }
    }

    WINDOWS_EXPORT void canvas_set_shadow_offsetY(void* ctxPtr, float oY) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;

        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );

        if (ctx->currentState.shadowBlurOffsetY == oY) return;
        ctx->currentState.shadowBlurOffsetY = oY;

        // Rebuild filters only if a active shadow exists
        if (ctx->currentState.shadowBlurAmount > 0.2f) {
            updateCanvasFilters(ctx->currentState);
        }
    }

    WINDOWS_EXPORT bool canvas_set_shadow_color(void* renderingContext, const char* c) {
        auto* ctx = validated<BunCanvasRenderingContext2D>(renderingContext);
        if (!ctx) return false;

        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );

        auto col = parseCssColor(c);
        if (!col) return false;

        ctx->currentState.shadowBlurColor.setColor4f(*col);

        // Rebuild drop shadow filter with updated color if a shadow is active
        if (ctx->currentState.shadowBlurAmount > 0.2f) {
            updateCanvasFilters(ctx->currentState);
        }

        return true;
    }

    WINDOWS_EXPORT bool canvas_set_image_smoothing_enabled(void* ctxPtr, bool v){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        
        if (v){
            switch(ctx->currentState.samplingQuality) {
                case CanvasSmoothingQuality::Low: {
                    ctx->currentState.sampling = SkFilterMode::kLinear;
                }
                case CanvasSmoothingQuality::Medium: {
                    ctx->currentState.sampling = SkFilterMode::kLinear;
                }
                case CanvasSmoothingQuality::High: {
                    ctx->currentState.sampling = SkCubicResampler::Mitchell();
                }
            }
            return true;
        } else {
            ctx->currentState.sampling = SkFilterMode::kNearest;
            return true;
        }
    }

    WINDOWS_EXPORT bool canvas_set_image_smoothing_quality(void* ctxPtr, CanvasSmoothingQuality v){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        switch(v) {
            case CanvasSmoothingQuality::Low: {
                ctx->currentState.sampling = SkFilterMode::kLinear;
                ctx->currentState.samplingQuality = v;
                return true;
            }
            case CanvasSmoothingQuality::Medium: {
                ctx->currentState.sampling = SkFilterMode::kLinear;
                ctx->currentState.samplingQuality = v;
                return true;
            }
            case CanvasSmoothingQuality::High: {
                ctx->currentState.sampling = SkCubicResampler::Mitchell();
                ctx->currentState.samplingQuality = v;
                return true;
            }
        }
        return false;
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

    WINDOWS_EXPORT void canvas_destroy_all() {
        for (auto* cnv : canvases) {
            if (!cnv) continue;
            std::lock_guard<std::mutex> lock(cnv->mutex);
            cnv->surface.reset();
            #ifndef __APPLE__
            if (mainThreadCtx && mainThreadCtx->context) {
                if (cnv->hasBackendTex) {
                    mainThreadCtx->context->deleteBackendTexture(cnv->backendTex);
                    cnv->hasBackendTex = false;
                }
                for (auto& r : cnv->retiredTextures) {
                    mainThreadCtx->context->deleteBackendTexture(r.tex);
                }
                cnv->retiredTextures.clear();
            }
            #endif
        }
        canvases.clear();
    }

    WINDOWS_EXPORT void canvas_get_transform(void* ctxPtr, float* mtx){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        
        auto matrix = (*ctx)()->getLocalToDevice();
        matrix.getRowMajor(mtx);
    }
    
    WINDOWS_EXPORT void canvas_set_transform(void* ctxPtr, double* mtx){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        (*ctx)()->resetMatrix();
        (*ctx)()->setMatrix(SkM44{
            (float)mtx[0],  (float)mtx[1],  (float)mtx[2],  (float)mtx[3],
            (float)mtx[4],  (float)mtx[5],  (float)mtx[6],  (float)mtx[7],
            (float)mtx[8],  (float)mtx[9],  (float)mtx[10], (float)mtx[11],
            (float)mtx[12], (float)mtx[13], (float)mtx[14], (float)mtx[15]
        });

        // SkImageFilters::Compose
    }
    WINDOWS_EXPORT void canvas_reset_transform(void* ctxPtr){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        (*ctx)()->resetMatrix();
    }

    WINDOWS_EXPORT void canvas_set_line_dash(void* ctxPtr, float* pts, int len, float offset){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );

        if(!pts || len <= 0) ctx->currentState.strokeColor.setPathEffect(nullptr);
        ctx->currentState.lineDashEffect = SkDashPathEffect::Make(SkSpan<const SkScalar>(pts, len),offset);
        ctx->currentState.strokeColor.setPathEffect(ctx->currentState.lineDashEffect);
    }

    WINDOWS_EXPORT bool canvas_set_line_join(void* ctxPtr, SkPaint::Join lJoinType){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        ctx->currentState.strokeColor.setStrokeJoin(lJoinType);
        return true;
    }

    WINDOWS_EXPORT bool canvas_set_text_baseline(void* ctxPtr, CanvasTextBaseLine bl){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        ctx->currentState.textBaseline = bl;
        return true;
    }

    WINDOWS_EXPORT bool canvas_set_text_align(void* ctxPtr, CanvasTextAlignment al){
        auto* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx) return false;
        nonapple(
            std::optional<std::lock_guard<std::mutex>> lock;
            if (!ctx->desynchronized) lock.emplace(ctx->owner->mutex);
        );
        ctx->currentState.textAlign = al;
        return true;
    }

    WINDOWS_EXPORT void canvas_measure_text(void* ctxPtr, const char* txt, float* fMetrics){
        BunCanvasRenderingContext2D* ctx = validated<BunCanvasRenderingContext2D>(ctxPtr);
        if (!ctx || !txt || !fMetrics) {
            std::cout << "measure_text returned early!\n";
            return;
        };
        SkRect bounds;
        float width = ctx->currentState.font_current.measureText(
            txt,
            std::strlen(txt),
            SkTextEncoding::kUTF8, &bounds
        );
        
        SkPaint fColor = ctx->currentState.fillColor;
        auto alphaf = fColor.getAlphaf() * ctx->currentState.globalAlpha;
        fColor.setAlphaf(alphaf);

        SkFontMetrics m;
        ctx->currentState.font_current.getMetrics(&m);

        fMetrics[0] = width;
        fMetrics[1] = bounds.fLeft;
        fMetrics[2] = bounds.fRight;
        fMetrics[3] = -bounds.fTop;
        fMetrics[4] = bounds.fBottom;
        fMetrics[5] = -m.fAscent;
        fMetrics[6] = m.fDescent;
        // fMetrics[7] TODO
        // fMetrics[8] TODO
        fMetrics[9] = ctx->currentState.textBaseline == CanvasTextBaseLine::Alphabetic;
        fMetrics[10] = ctx->currentState.textBaseline == CanvasTextBaseLine::Hanging;
        fMetrics[11] = ctx->currentState.textBaseline == CanvasTextBaseLine::Ideographic;
    }
}