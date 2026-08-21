class ImageWrapper {
    public:
    sk_sp<SkData> data;
    sk_sp<SkImage> image;
    std::string src;
    bool imageValid = false;
    bool isFromURL = false;

    static constexpr uint64_t MAGIC = 0x537119B1;
    uint64_t magic = MAGIC;

    ImageWrapper(){}
};

std::vector<std::string> tmpImages;

extern "C" {
    WINDOWS_EXPORT void* image_create() {
        return new ImageWrapper();
    }

    WINDOWS_EXPORT void images_cleanup() {
        for (auto img : tmpImages) {
            if (!img.empty()) {
                std::error_code ec;
                std::filesystem::remove(img, ec);
            }
        }
    }

    WINDOWS_EXPORT void image_set_src(void* imgObj, const char* path, bool isFromURL = false){
        ImageWrapper* obj = validated<ImageWrapper>(imgObj);
        if (!obj) return;

        obj->imageValid = false;
        obj->src = path;
        obj->isFromURL = isFromURL;
        if (isFromURL) {
            tmpImages.push_back(path);    
        }
    }

    // WINDOWS_EXPORT void image_set_src_url(void* imgObj, const char* path, bool isFromURL = false){
    //     ImageWrapper* obj = validated<ImageWrapper>(imgObj);
    //     if (!obj) return;

    //     obj->isFromURL = isFromURL;
    //     obj->imageValid = false;
    //     obj->src = path;
    // }

    WINDOWS_EXPORT bool image_load(void* imgObj){
        ImageWrapper* obj = validated<ImageWrapper>(imgObj);
        if (!obj || obj->src.empty()) return false;
        if (obj->imageValid) return true;
        obj->data.reset();
        obj->data = SkData::MakeFromFileName(obj->src.c_str());
        if(obj->data) {
            obj->imageValid = true;
            auto img = SkImages::DeferredFromEncodedData(obj->data);
            if (img){
                obj->image = gpuContextReady ? SkImages::TextureFromImage(mainThreadCtx->context.get(),img) : img;
            }
            return true;
        }
        return false;
    }

    WINDOWS_EXPORT void image_destroy(void* ptr) {
        ImageWrapper* obj = validated<ImageWrapper>(ptr);
        // if (obj->isFromURL && !obj->src.empty()) {
        //     std::error_code ec;
        //     std::filesystem::remove(obj->src, ec);
        //     std::cout << "removed: " << obj->src << "\n";
        // }
        // std::cout << "destroyed!\n";
        delete obj;
    }
}