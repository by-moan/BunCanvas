#include <iostream>
#include <vector>
#include <mutex>
#include <queue>

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

#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkData.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkColorSpace.h"

#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/SkImageGanesh.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"

#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
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



#include "src/Image.cpp"
#include "src/Canvas.cpp"
#include "src/Window.cpp"