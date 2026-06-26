class ImageWrapper {
    public:
    sk_sp<SkData> data;
    sk_sp<SkImage> image;
    bool imageValid = false;

    static constexpr uint64_t MAGIC = 0x537119B1;
    uint64_t magic = MAGIC;

    ImageWrapper(){}
};

ImageWrapper* validated_image(void* imagePtr){
    ImageWrapper* obj = static_cast<ImageWrapper*>(imagePtr);
    
    if (obj->magic != ImageWrapper::MAGIC)
    return nullptr;
    
    
    return obj;
}

extern "C" {
    void* image_create() {
        return new ImageWrapper();
    }

    bool image_set_src(void* imgObj, const char* path){
        ImageWrapper* obj = validated_image(imgObj);
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
}