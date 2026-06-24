class ImageWrapper {
    public:
    sk_sp<SkData> data;
    sk_sp<SkImage> image;

    ImageWrapper(const char* path) : data(SkData::MakeFromFileName(path)){
        if (data) {
            image = SkImages::DeferredFromEncodedData(data);
        }
    }
};

extern "C" {
    void* image_load(const char* path) {
        return new ImageWrapper(path);
    }
}