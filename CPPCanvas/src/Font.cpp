#include "CssFontParser.cpp"


static std::mutex cacheMtx;
static std::unordered_map<std::string, sk_sp<SkTypeface>> fontRegistry;
static std::mutex fontCacheMtx;
static std::unordered_map<std::string, sk_sp<SkTypeface>> fontCache;

extern "C" {
    WINDOWS_EXPORT void* font_create_from_face(
        const char* family,
        const char* srcPath,
        int32_t weight,
        int32_t style,
        int32_t stretch,
        const char* unicodeRange,
        const char* variant,
        const char* featureSettings
    ) {
        SkFontArguments args;
        SkFontStyle::Weight w = static_cast<SkFontStyle::Weight>(weight);
        SkFontStyle::Width  wd = static_cast<SkFontStyle::Width>(stretch);
        SkFontStyle::Slant  sl = static_cast<SkFontStyle::Slant>(style);
        SkFontStyle styleObj(w, wd, sl);
        
        sk_sp<SkTypeface> tf;
        {

            auto stream = SkStream::MakeFromFile(srcPath);
            if (stream == nullptr) return nullptr;

            SkFontArguments args;
            args.setCollectionIndex(0);

            SkFontArguments::VariationPosition::Coordinate coords[] = {
                { SkSetFourByteTag('w', 'g', 'h', 't'), (float)weight },
                { SkSetFourByteTag('w', 'd', 't', 'h'), (float)width },
            };

            SkFontArguments::VariationPosition pos;
            pos.coordinates = coords;
            pos.coordinateCount = std::size(coords);
            args.setVariationDesignPosition(pos);

            tf = fontMgr->makeFromStream(std::move(stream), args);
        }
        
        // uint64_t key = reinterpret_cast<uint64_t>(tf.get());
        
        return tf.release();
    }

    void add_font_to_registry(const char* fFamily, void* ptr){
        if(!ptr) {
            std::cout << "No pointer!\n";
            return;
        }
        SkTypeface* fPtr = static_cast<SkTypeface*>(ptr);
        if(!fPtr) {
            std::cout << "failed cast!\n";
            return;
        }
        {
            std::lock_guard<std::mutex> lk(cacheMtx);
            std::string s = fFamily;
            std::transform(s.begin(), s.end(), s.begin(), tolower);
            if (!fontRegistry.contains(s)) {
                fontRegistry.emplace(s, sk_sp<SkTypeface>(fPtr));
                std::cout << "font placed!\n";
            }
        }
    }
    
    WINDOWS_EXPORT bool font_load(void* facePtr) {
        return facePtr != nullptr;
    }
    
}