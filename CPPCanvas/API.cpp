#include <iostream>
#include <vector>

#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkColorSpace.h"

#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"

#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"

void framebuffer_size_callback(
    GLFWwindow* window,
    int width,
    int height
) {
    std::cout << "Window resized!\n";
    glViewport(0, 0, width, height);
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

// int main() {
//     glfwInit();
    
//     window =
//     glfwCreateWindow(800, 600, "Skia", nullptr, nullptr);
    
//     glfwMakeContextCurrent(window);
    
//     auto iface = GrGLMakeNativeInterface();
    
//     if (!iface) {
//         std::cout << "Native interface failed\n";
//         return -1;
//     }
    
//     std::cout << "Native interface OK\n";
    
//     ctxWrapper = new ContextWrapper(GrDirectContexts::MakeGL(iface));
//     if (!ctxWrapper->context) {
//         std::cout << "Context was not created!\n";
//         return -1;
//     };
    
//     GrGLFramebufferInfo fbInfo;
//     fbInfo.fFBOID = 0;
//     fbInfo.fFormat = GL_RGBA8;
    
    
//     glfwGetFramebufferSize(window, &width, &height);
    
//     currentWidth = width;
//     currentHeight = height;
    
//     sWrapper = new SurfaceWrapper(createSurface(
//         ctxWrapper->context.get(),
//         width,
//         height
//     ));
    
//     if (!sWrapper->surface) {
//         std::cout << "Surface was not created!\n";
//         return -1;
//     };
    
//     glfwGetFramebufferSize(window, &width, &height);

//     canvas = sWrapper->surface->getCanvas();
    
//     while (!glfwWindowShouldClose(window)) {
//         glfwPollEvents();
//         int width, height;
//         glfwGetFramebufferSize(window, &width, &height);
        
//         if (width != currentWidth || height != currentHeight) {
//             currentWidth = width;
//             currentHeight = height;
            
//             glViewport(0, 0, width, height);
            
//             ctxWrapper->context->flush();
//             sWrapper->surface.reset();
//             sWrapper->surface = createSurface(
//                 ctxWrapper->context.get(),
//                 width,
//                 height
//             );
//             canvas = sWrapper->surface->getCanvas();
//         }
        
//         canvas->clear(SK_ColorTRANSPARENT);
        
//         SkPaint p;
//         p.setColor(SK_ColorRED);
//         p.setAntiAlias(true);
        
//         canvas->drawRect(
//             SkRect::MakeXYWH(400, 200, 400, 400),
//             p
//         );
        
//         ctxWrapper->context->flush();
//         glfwSwapBuffers(window);
//     }
// }