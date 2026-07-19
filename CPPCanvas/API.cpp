#include <iostream>
#include <vector>
#include <mutex>
#include <cctype>
#include <regex>
#include <algorithm>
// #include <queue>
#include <string>
#include <string_view>
#include <unordered_map>

// #include <pthread.h>

#ifdef _WIN64

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _Float64_t double
#include <windows.h>



// This... This is the worst thing humanity would create.... Why??????
#define WINDOWS_EXPORT __declspec(dllexport)

#else
#define WINDOWS_EXPORT
#endif


#ifndef __APPLE__
#define nonapple(code) code
#include <GL/glew.h>
#include <GL/gl.h>
#else
#define _Float64_t double
#define nonapple(code)
#endif

#include <GLFW/glfw3.h>

#include "include/core/SkFontScanner.h"
#include "include/core/SkFontArguments.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkFourByteTag.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkBlurTypes.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkShader.h"
#include "include/effects/SkGradient.h"
#include "include/effects/SkImageFilters.h"
#include "include/core/SkPath.h"
#include "include/core/SkSpan.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkData.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkStream.h"

#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/SkImageGanesh.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"

#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"

#ifdef __linux__
#include "include/ports/SkFontMgr_fontconfig.h"   // Linux
#include "include/ports/SkFontScanner_FreeType.h"
#elif __APPLE__
#include "include/ports/SkFontMgr_mac_ct.h"
#elif _WIN64
#include "include/ports/SkFontMgr_win.h"
#else
#error "Platform is not supported by BunCanvas"
#endif

sk_sp<SkFontMgr> CreateFontManager() {
#ifdef __linux__
    return SkFontMgr_New_FontConfig(
        nullptr,
        SkFontScanner_Make_FreeType());

#elif defined(__APPLE__)
    CTFontCollectionRef collection =
        CTFontCollectionCreateFromAvailableFonts(nullptr);

    auto mgr = SkFontMgr_New_CoreText(collection);

    CFRelease(collection);

    return mgr;

#elif defined(_WIN64)
    IDWriteFactory* factory = nullptr;

    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&factory));

    IDWriteFontCollection* collection = nullptr;
    factory->GetSystemFontCollection(&collection);

    auto mgr = SkFontMgr_New_DirectWrite(factory, collection);

    collection->Release();
    factory->Release();

    return mgr;

#else
#endif
}

sk_sp<SkSurface> createSurface(
    GrDirectContext* context,
    int width,
    int height
) {
    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = 0;
    fbInfo.fFormat = GL_RGBA8;
    
    auto backendRT = GrBackendRenderTargets::MakeGL(
        width,
        height,
        0,
        8,
        fbInfo
    );
    return SkSurfaces::WrapBackendRenderTarget(
        context,
        backendRT,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        nullptr,
        nullptr
    );
}

class SurfaceWrapper {
	public:
	sk_sp<SkSurface> surface;
	SurfaceWrapper(sk_sp<SkSurface> sfc) : surface(sfc){}
};

class ContextWrapper {
	public:
	sk_sp<GrDirectContext> context;
	ContextWrapper(sk_sp<GrDirectContext> ctx) : context(ctx){}
};


class InterfaceWrapper {
    public:
    sk_sp<const GrGLInterface> interface;

    InterfaceWrapper(sk_sp<const GrGLInterface> iface) : interface(iface){}
};

InterfaceWrapper* renderThreadInterface = nullptr;
ContextWrapper* renderThreadContext = nullptr;

int currentWidth = 0;
int currentHeight = 0;
int width, height;
SkCanvas* canvas = nullptr;
GLFWwindow* window = nullptr;
SurfaceWrapper* sWrapper = nullptr;
sk_sp<SkFontMgr> fontMgr = CreateFontManager();
bool ready = false;
bool mainThreadReady = false;

#ifndef __APPLE__
GLFWwindow* sharedWindow = nullptr;
ContextWrapper* mainThreadCtx = nullptr;
InterfaceWrapper* mainThreadIface = nullptr;
bool gpuContextReady = false;
#endif

std::recursive_mutex loop_mutex;

static int32_t wResizeViewer[3]{};
static _Float64_t mMoveViewer[9]{};
static _Float64_t mClickViewer[9]{};
static _Float64_t mDownViewer[9]{};
static _Float64_t mUpViewer[9]{};
static int32_t kDownViewer[7]{};
static int32_t kUpViewer[7]{};


extern "C" {
    WINDOWS_EXPORT int32_t* get_wResizeViewer(){return wResizeViewer;}
    WINDOWS_EXPORT _Float64_t* get_mMoveViewer(){return mMoveViewer;}
    WINDOWS_EXPORT _Float64_t* get_mClickViewer(){return mClickViewer;}
    WINDOWS_EXPORT _Float64_t* get_mDownViewer(){return mDownViewer;}
    WINDOWS_EXPORT _Float64_t* get_mUpViewer(){return mUpViewer;}
    WINDOWS_EXPORT int32_t* get_kDownViewer(){return kDownViewer;}
    WINDOWS_EXPORT int32_t* get_kUpViewer(){return kUpViewer;}
}

// For this function to compile the object to convert MUST have the following components: static constexpr uint64_t MAGIC and uint64_t magic
template <typename targetPtr>
targetPtr* validated(void* ptr){
    if (!ptr) return nullptr;
    targetPtr* obj = static_cast<targetPtr*>(ptr);
        
    if (obj->magic != targetPtr::MAGIC)
    return nullptr;
        
        
    return obj;
}

// #include "src/FontFace/FontCache.cpp"
// #include "src/FontFace/FontFaceSet.cpp"
#include "src/Font.cpp"
#include "src/Image.cpp"
#include "src/Canvas.cpp"
#include "src/Window.cpp"