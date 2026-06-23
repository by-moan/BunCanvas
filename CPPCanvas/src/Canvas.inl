class BunCanvas;



class BunCanvas {
    public:
    static constexpr uint64_t MAGIC = 0xBCA1155A;
    uint64_t magic = MAGIC;
    
    sk_sp<SkSurface> surface;
    SkCanvas* ctx = nullptr;
    SkPaint fillColor;
    SkPaint strokeColor;
    SkPaint clearColor;
    SkPathBuilder pathBuilder;
    SkPath path;

    //Lock in case a sensitive state, e.g. resizing is ongoing
    bool locked = false;
    
    BunCanvas(int w, int h) : surface(SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w,h))) {
        strokeColor.setColor(SK_ColorBLACK);
        strokeColor.setStyle(SkPaint::kStroke_Style);
        strokeColor.setStrokeWidth(1);
        strokeColor.setAntiAlias(1);
        fillColor.setColor(SK_ColorBLACK);
        fillColor.setStyle(SkPaint::kFill_Style);
        fillColor.setAntiAlias(1);

        clearColor.setColor(SK_ColorTRANSPARENT);
        clearColor.setStyle(SkPaint::kFill_Style);
        clearColor.setBlendMode(SkBlendMode::kClear);
        clearColor.setAntiAlias(1);
    }
};

std::vector<BunCanvas*> canvases;

BunCanvas* validated(void* ptr){
    BunCanvas* obj = static_cast<BunCanvas*>(ptr);

    if (obj->magic != BunCanvas::MAGIC)
    return nullptr;


    return obj;
}

extern "C" {
    
    void* canvas_create(int w, int h) {
        return new BunCanvas(w,h);
    }

    void canvas_append(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvas* obj = static_cast<BunCanvas*>(canvasObj);
        if (obj->magic != BunCanvas::MAGIC)
            return;

        canvases.push_back(obj);
    }
    
    //Sets native internal context so it is returned if is asked again.
    void canvas_setup_context(void* canvasObj){
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;
        

        obj->ctx = obj->surface->getCanvas();
    }
    
    void canvas_set_fill_style(void* canvasObj, const char* c) {
        if (!canvasObj) return;
        
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;
        
        unsigned int rgb = std::strtoul(c+1, nullptr, 16);
        
        obj->fillColor.setColor(SkColor4f{
            ((rgb >> 16) & 0xFF) / 255.f,
            ((rgb >> 8) & 0xFF) / 255.f,
            (rgb & 0xFF) / 255.f,
            1.0f
        });
    }
    
    void canvas_set_stroke_style(void* canvasObj, const char* c) {
        if (!canvasObj) return;
        
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;
        
        unsigned int rgb = std::strtoul(c+1, nullptr, 16);
        
        obj->strokeColor.setColor(SkColor4f{
            ((rgb >> 16) & 0xFF) / 255.f,
            ((rgb >> 8) & 0xFF) / 255.f,
            (rgb & 0xFF) / 255.f,
            1.0f
        });
    }

    void canvas_set_stroke_width(void* canvasObj, float w) {
        if (!canvasObj) return;
        
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        obj->strokeColor.setStrokeWidth(w);
    }
    
    void canvas_fill_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;
        
        obj->ctx->drawRect(SkRect::MakeXYWH(x,y,w,h), obj->fillColor);
    }

    void canvas_clear_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;

        BunCanvas* obj = validated(canvasObj);
        if (!obj) return;

        SkPaint p;
        p.setBlendMode(SkBlendMode::kClear);

        obj->ctx->save();
        obj->ctx->clipRect(SkRect::MakeXYWH(x, y, w, h));
        obj->ctx->drawPaint(p);
        obj->ctx->restore();
    }
    
    void canvas_resize(void* canvasObj, int w, int h) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        obj->locked = true;
        obj->surface.reset();
        obj->surface = SkSurfaces::Raster(
            SkImageInfo::MakeN32Premul(w,h)
        );
        obj->ctx = obj->surface->getCanvas();
        obj->locked = false;
    }

    void canvas_path_begin(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;
        obj->path.reset();
        obj->pathBuilder.reset();
    }

    void canvas_path_move_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        obj->pathBuilder.moveTo(x,y);
    }
    void canvas_path_line_to(void* canvasObj, int x, int y) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        obj->pathBuilder.lineTo(x,y);
    }

    void canvas_path_arc(void* canvasObj, float x1, float y1, float radius, float startAngle, float sweepangle) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        obj->pathBuilder.addArc(SkRect::MakeXYWH(x1,y1,radius*2,radius*2), startAngle, sweepangle);
    }

    void canvas_path_arc_to(void* canvasObj, float x1, float y1, float x2, float y2, float radius) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        // Parameters:
        // oval – bounds of ellipse containing arc
        // startAngleDeg – starting angle of arc in degrees
        // sweepAngleDeg – sweep, in degrees. Positive is clockwise; treated modulo 360
        // forceMoveTo – true to start a new contour with arc
        obj->pathBuilder.arcTo({x1,y1},{x2,y2},radius);
    }

    void canvas_path_bezier_to(void* canvasObj, float x1, float y1, float x2, float y2, float x3, float y3) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        // Parameters:
        // oval – bounds of ellipse containing arc
        // startAngleDeg – starting angle of arc in degrees
        // sweepAngleDeg – sweep, in degrees. Positive is clockwise; treated modulo 360
        // forceMoveTo – true to start a new contour with arc
        obj->pathBuilder.cubicTo(x1,y1,x2,y2,x3,y3);
    }
    void canvas_path_stroke(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) {
            return;
        };
        obj->ctx->drawPath(obj->pathBuilder.snapshot(), obj->strokeColor);
    }

    void canvas_path_close(void* canvasObj) {
        if (!canvasObj) return;
        BunCanvas* obj = validated(canvasObj);

        if (obj == nullptr) return;

        obj->pathBuilder.close();
    }
}