class ImageWrapper {
    public:
    sk_sp<SkData> data;
    sk_sp<SkImage> image;
    bool imageValid = false;

    static constexpr uint64_t MAGIC = 0x537119B1;
    uint64_t magic = MAGIC;

    ImageWrapper(){}
};

extern "C" {
    WINDOWS_EXPORT void* image_create() {
        return new ImageWrapper();
    }

    WINDOWS_EXPORT bool image_set_src(void* imgObj, const char* path){
        ImageWrapper* obj = validated<ImageWrapper>(imgObj);
        if (!obj) return false;

        obj->imageValid = false;
        obj->data.reset();
        obj->data = SkData::MakeFromFileName(path);
        if(obj->data) {
            obj->imageValid = true;
            obj->image = SkImages::DeferredFromEncodedData(obj->data);
            return true;
        }
        return false;
    }

    WINDOWS_EXPORT void image_destroy(void* ptr) {
        ImageWrapper* obj = validated<ImageWrapper>(ptr);
        delete obj;
    }
}