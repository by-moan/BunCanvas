// #include <iostream>
// #include <vector>
// #include <string>
// #include <mutex>
// #include <memory>
// #include <algorithm>

// #define MINIAUDIO_IMPLEMENTATION
// #include "miniaudio.h"

// Represents a single active playback instance
struct AudioVoice {
    ma_decoder decoder;
    bool active = true;

    ~AudioVoice() {
        ma_decoder_uninit(&decoder);
    }
};

class AudioBackend {
    bool valid = false;
    bool hasInit = false;
    bool started = false;
public:
    bool isValid(){
        return valid;
    }

    AudioBackend() = default;

    void init() {
        // Initialize the device
        if (hasInit) return;
        hasInit = true;
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format   = ma_format_f32;
        config.playback.channels = 2;
        config.sampleRate        = 44100;
        
        // The standard way to get audio is via data_callback
        config.dataCallback     = AudioBackend::dataCallback;
        config.pUserData         = this;

        if (ma_device_init(NULL, &config, &device) == MA_SUCCESS) {
            std::cout << "ma_device_init  success!\n";
            valid = true;
        }else {
            std::cout << "Fail!!!\n";
        }
    }

    ~AudioBackend() {
        ma_device_uninit(&device);
    }

    void start() {
        if (started) return;

        ma_result result = ma_device_start(&device);

        if (result != MA_SUCCESS) {
            std::cerr << "ma_device_start failed: "
                    << result << '\n';
            return;
        }

        started = true;
        std::cout << "Audio device started!\n";
    }

    void addVoice(std::unique_ptr<AudioVoice> voice) {
        std::lock_guard<std::mutex> lock(voiceMutex);
        activeVoices.push_back(std::move(voice));
    }

    static void dataCallback(
        ma_device* pDevice,
        void* pOutput,
        const void* pInput,
        ma_uint32 frameCount
    ) {
        AudioBackend* backend =
            static_cast<AudioBackend*>(pDevice->pUserData);

        float* out = static_cast<float*>(pOutput);

        const ma_uint32 channels = pDevice->playback.channels;

        // Silence output first.
        std::fill(
            out,
            out + frameCount * channels,
            0.0f
        );

        std::lock_guard<std::mutex> lock(backend->voiceMutex);

        for (auto it = backend->activeVoices.begin();
            it != backend->activeVoices.end();) {

            std::vector<float> tempBuffer(
                frameCount * channels
            );

            ma_uint64 framesRead = 0;

            ma_result result = ma_decoder_read_pcm_frames(
                &((*it)->decoder),
                tempBuffer.data(),
                frameCount,
                &framesRead
            );

            if (result != MA_SUCCESS || framesRead == 0) {
                it = backend->activeVoices.erase(it);
                continue;
            }

            // Mix into output.
            for (ma_uint64 i = 0; i < framesRead * channels; ++i) {
                out[i] += tempBuffer[i];
            }

            ++it;
        }
    }

    ma_device device;
    std::vector<std::unique_ptr<AudioVoice>> activeVoices;
    std::mutex voiceMutex;
};

class AudioObject {
    AudioBackend& backend;
    std::string path;
    bool hasSrc;
public:

    static constexpr uint64_t MAGIC = 0x15EBD88E;
    uint64_t magic = MAGIC;

    AudioObject(AudioBackend& backend, const std::string& filePath) 
        : backend(backend), path(filePath), hasSrc(true) {}
    AudioObject(AudioBackend& backend) 
        : backend(backend), hasSrc(false) {}

    void play() {
        if (!backend.isValid() || !hasSrc) {
            return;
        };
        auto voice = std::make_unique<AudioVoice>();
        
        // Use standard decoder config
        ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 44100);
        
        if (ma_decoder_init_file(path.c_str(), &decoderConfig, &voice->decoder) != MA_SUCCESS) {
            std::cerr << "Failed to load file: " << path << std::endl;
            return;
        }
        std::cout << "playing!\n";

        backend.addVoice(std::move(voice));
    }

    void setSource(const std::string& newPath) {
        path = newPath;
    }
};

static AudioBackend globalAudioBackend;

extern "C" {

    void audio_backend_init(){
        globalAudioBackend.init();
        globalAudioBackend.start();
    }

    AudioObject* audio_create(const char* path){
        if (path) {
            return new AudioObject(globalAudioBackend,path);

        }
        return new AudioObject(globalAudioBackend);
    }

    void audio_play(void* ptr){
        auto obj = validated<AudioObject>(ptr);
        if (!obj) return;
        obj->play();
    }
    
    void audio_set_src(void* ptr, const char* path){
        auto obj = validated<AudioObject>(ptr);
        if (!obj) return;
        obj->setSource(path);    
    }

    void audio_destroy(void* ptr){
        auto obj = validated<AudioObject>(ptr);
        if (obj) delete obj; 
    }

}
// int main() {
//     try {
//         AudioBackend backend;
//         backend.start();

//         AudioObject sound1(backend, "test.wav");
        
//         std::cout << "Playing... Press Enter to quit." << std::endl;
//         sound1.play();

//         std::cin.get();
//     } catch (const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
//     return 0;
// }