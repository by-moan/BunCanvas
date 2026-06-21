class BunCanvas;


class BunCanvas {
    public:
    static constexpr uint64_t MAGIC = 0xBCA1155A;
    uint64_t magic = MAGIC;
    
    sk_sp<SkSurface> surface;
    SkCanvas* ctx = nullptr;
    SkPaint fCol;

    //Lock in case a sensitive state, e.g. resizing is ongoing
    bool locked = false;
    
    BunCanvas(int w, int h) : surface(SkSurfaces::Raster(SkImageInfo::MakeN32Premul(w,h))) {}
};

std::vector<BunCanvas*> canvases;

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
        BunCanvas* obj = static_cast<BunCanvas*>(canvasObj);
        if (obj->magic != BunCanvas::MAGIC)
        return;

        obj->ctx = obj->surface->getCanvas();
    }
    
    void canvas_set_fill_style(void* canvasObj, const char* c) {
        if (!canvasObj) return;
        
        BunCanvas* obj = static_cast<BunCanvas*>(canvasObj);
        
        if (obj->magic != BunCanvas::MAGIC || obj->locked == true)
        return;
        
        unsigned int rgb = std::strtoul(c+1, nullptr, 16);
        
        obj->fCol.setColor(SkColor4f{
            ((rgb >> 16) & 0xFF) / 255.f,
            ((rgb >> 8) & 0xFF) / 255.f,
            (rgb & 0xFF) / 255.f,
            1.0f
        });
    }
    
    void canvas_fill_rect(void* canvasObj, int x, int y, int w, int h) {
        if (!canvasObj) return;
        BunCanvas* obj = static_cast<BunCanvas*>(canvasObj);
        
        if (obj->magic != BunCanvas::MAGIC || obj->locked == true)
        return;
        
        obj->ctx->drawRect(SkRect::MakeXYWH(x,y,w,h), obj->fCol);
    }
    
    void canvas_resize(void* canvasObj, int w, int h) {
        if (!canvasObj) return;
        BunCanvas* obj = static_cast<BunCanvas*>(canvasObj);
        
        if (obj->magic != BunCanvas::MAGIC || obj->locked == true)
        return;

        obj->locked = true;

        obj->surface.reset();

        obj->surface = SkSurfaces::Raster(
            SkImageInfo::MakeN32Premul(w,h)
        );
        obj->ctx = obj->surface->getCanvas();

        obj->locked = false;
    }
}