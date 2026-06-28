#include <iostream>
#include <vector>
#include <pthread.h>

#ifndef __APPLE__
#include <GL/gl.h>
#else
#define _Float64_t double
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

int currentWidth = 0;
int currentHeight = 0;
int width, height;
SkCanvas* canvas = nullptr;
GLFWwindow* window = nullptr;
SurfaceWrapper* sWrapper = nullptr;
ContextWrapper* ctxWrapper = nullptr;

#include "src/Image.inl"
#include "src/Canvas.inl"
#include "src/Window.inl"