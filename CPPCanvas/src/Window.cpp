bool pollingEvents = false;

bool pendingResize = false;
int pendingW = 1;
int pendingH = 1;
void window_resize_callback(GLFWwindow* window, int w, int h) {
    pendingResize = true;
    pendingW = w;
    pendingH = h;
}

double prevX = 0;
double prevY = 0;

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (pollingEvents == true) return;
    mMoveViewer[0] = true;
    mMoveViewer[1] = xpos;
    mMoveViewer[2] = ypos;
    mMoveViewer[3] = xpos - prevX;
    mMoveViewer[4] = ypos - prevY;
    prevX = xpos;
    prevY = ypos;
}

bool clickStarted = false;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (pollingEvents == true) return;
    
    if (action == GLFW_PRESS) { //Indicates that the mouse was pressed down.
        clickStarted = (button == GLFW_MOUSE_BUTTON_LEFT)?true:false;
        mClickViewer[4] += (button == GLFW_MOUSE_BUTTON_LEFT && !((int)mClickViewer[4]&0b1))?0b1:0;
        mClickViewer[4] += (button == GLFW_MOUSE_BUTTON_RIGHT && !((int)mClickViewer[4]&0b10))?0b10:0;
        mClickViewer[4] += (button == GLFW_MOUSE_BUTTON_MIDDLE && !((int)mClickViewer[4]&0b100))?0b100:0;
        //Backwards mouse navigation button
        mClickViewer[4] += (button == GLFW_MOUSE_BUTTON_4 && !((int)mClickViewer[4]&0b1000))?0b1000:0;
        // Forward mouse navigation button
        mClickViewer[4] += (button == GLFW_MOUSE_BUTTON_5 && !((int)mClickViewer[4]&0b10000))?0b10000:0;
        //Buttons field for mouseMove viewer
        mMoveViewer[5] = mClickViewer[4];

        mDownViewer[0] = true;
        mDownViewer[1] = mMoveViewer[1];
        mDownViewer[2] = mMoveViewer[2];
        mDownViewer[4] = mClickViewer[4];
        mDownViewer[5] = kDownViewer[3];
        mDownViewer[6] = kDownViewer[4];
        mDownViewer[7] = kDownViewer[5];
        mDownViewer[8] = kDownViewer[6];
    } 
    if (action == GLFW_RELEASE) {
        mClickViewer[4] -= (button == GLFW_MOUSE_BUTTON_LEFT && (int)mClickViewer[4]&0b1)?0b1:0;
        mClickViewer[4] -= (button == GLFW_MOUSE_BUTTON_RIGHT && (int)mClickViewer[4]&0b10)?0b10:0;
        mClickViewer[4] -= (button == GLFW_MOUSE_BUTTON_MIDDLE && (int)mClickViewer[4]&0b100)?0b100:0;
        //Backwards mouse navigation button
        mClickViewer[4] -= (button == GLFW_MOUSE_BUTTON_4 && (int)mClickViewer[4]&0b1000)?0b1000:0;
        // Forward mouse navigation button
        mClickViewer[4] -= (button == GLFW_MOUSE_BUTTON_5 && (int)mClickViewer[4]&0b10000)?0b10000:0;
        //Buttons field for mouseMove viewer
        mMoveViewer[5] = mClickViewer[4];

        mUpViewer[0] = true;
        mUpViewer[1] = mMoveViewer[1];
        mUpViewer[2] = mMoveViewer[2];
        mUpViewer[4] = mClickViewer[4];
        mUpViewer[5] = kDownViewer[3];
        mUpViewer[6] = kDownViewer[4];
        mUpViewer[7] = kDownViewer[5];
        mUpViewer[8] = kDownViewer[6];
    }
    //Mouse pressed down was done and now is being released? Then mark the event as dirty so it can be triggered on js window.
    if (button == GLFW_MOUSE_BUTTON_LEFT && clickStarted == true && action == GLFW_RELEASE) {
        mClickViewer[0] = true;
        clickStarted = false;
        mClickViewer[1] = mMoveViewer[1];
        mClickViewer[2] = mMoveViewer[2];
        
        //shift key pressed
        mClickViewer[5] = (mods&0b1)?true:false;
        //ctrl key pressed
        mClickViewer[6] = (mods&0b10)?true:false;
        //alt key pressed
        mClickViewer[7] = (mods&0b100)?true:false;
        //meta key pressed
        mClickViewer[8] = (mods&0b1000)?true:false;
    }
}


void keyboard_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS || action == 2) {
        kDownViewer[0] = true;
        kDownViewer[1] = false;
        kDownViewer[2] = key;
        
        //shift key pressed
        kDownViewer[3] = (mods&0b1)?true:false;
        //ctrl key pressed
        kDownViewer[4] = (mods&0b10)?true:false;
        //alt key pressed
        kDownViewer[5] = (mods&0b100)?true:false;
        //meta key pressed
        kDownViewer[6] = (mods&0b1000)?true:false;
        
    }
    if(action == 2) {
        kDownViewer[1] = true;
    }
    if(action == GLFW_RELEASE) {
        kUpViewer[0] = true;
        kUpViewer[1] = false;
        kUpViewer[2] = key;
        
        //shift key pressed
        kUpViewer[3] = (mods&0b1)?true:false;
        //ctrl key pressed
        kUpViewer[4] = (mods&0b10)?true:false;
        //alt key pressed
        kUpViewer[5] = (mods&0b100)?true:false;
        //meta key pressed
        kUpViewer[6] = (mods&0b1000)?true:false;
    }
}

void window_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    mWheelViewer[0] = true;
    mWheelViewer[1] = xoffset;
    mWheelViewer[2] = yoffset;
}

void render_composited_canvases(SkCanvas* targetCanvas) {
    for (auto element : canvases) {
        std::lock_guard<std::mutex> lock(element->mutex);
        #ifndef __APPLE__
        if (element->hasBackendTex) {
            auto img = SkImages::BorrowTextureFrom(
                renderThreadContext->context.get(),
                element->backendTex,
                kTopLeft_GrSurfaceOrigin,
                kN32_SkColorType,
                kPremul_SkAlphaType,
                nullptr
            );
            if (img) {
                targetCanvas->drawImageRect(
                    img,
                    SkRect::MakeXYWH(0, 0, img->width(), img->height()),
                    SkFilterMode::kNearest
                );
            }
        } else {
            targetCanvas->drawImage(element->surface->makeTemporaryImage(), 0, 0);
        }
        #else
        targetCanvas->drawImage(element->surface->makeTemporaryImage(), 0, 0);
        #endif
    }
}

void window_refresh_callback(GLFWwindow* window) {
    if (!ready || swapchainSurfaces.empty()) return;
    uint32_t imageIndex = 0;
    if (!VK::acquireNextImage(&imageIndex)) return;

    SkCanvas* frameCanvas = swapchainSurfaces[imageIndex]->surface->getCanvas();
    frameCanvas->clear(SK_ColorTRANSPARENT);
    render_composited_canvases(frameCanvas);

    VK::presentImage(
        renderThreadContext->context.get(),
        swapchainSurfaces[imageIndex]->surface.get(),
        imageIndex
    );
}

typedef int (*JSCallback_WRefresh)(int);
bool currentVsync = true;

extern "C" {
    WINDOWS_EXPORT void create_window(int w, int h, const char* title) {
    }

    WINDOWS_EXPORT void glfw_init() {
        if (!glfwInit()) {
            std::cerr << "Couldn't initialize GLFW...\n";
            return;
        }
        if (!glfwVulkanSupported()) {
            std::cerr << "Vulkan is not supported on this system...\n";
            return;
        }

        // Tell GLFW not to create an OpenGL context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        g_clearColor.setColor(SK_ColorTRANSPARENT);
        g_clearColor.setStyle(SkPaint::kFill_Style);
        g_clearColor.setBlendMode(SkBlendMode::kClear);
        g_clearColor.setAntiAlias(1);
        g_noAlphaClearColor.setColor(SK_ColorBLACK);
        g_noAlphaClearColor.setStyle(SkPaint::kFill_Style);
        g_noAlphaClearColor.setAntiAlias(1);
        pImageDataColor.setBlendMode(SkBlendMode::kSrc);

        std::cout << "[OK] glfw_init()\n";
    }

    WINDOWS_EXPORT void set_vsync(bool v) {
        vsyncQueue.push_back(v);
    }

    #ifndef __APPLE__
    // Called in the bun worker thread.
    WINDOWS_EXPORT void setup_render_thread(int w, int h, const char* title, JSCallback_WRefresh onrefresh, bool vsync) {
        width = w;
        height = h;
        currentVsync = vsync;

        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            std::cerr << "Couldn't initialize Window...\n";
            const char* desc;
            int code = glfwGetError(&desc);
            std::cerr << "GLFW error " << code << ": " << (desc ? desc : "unknown") << "\n";
            glfwTerminate();
            return;
        }

        glfwSetWindowSizeCallback(window, window_resize_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window, keyboard_key_callback);
        glfwSetScrollCallback(window, window_scroll_callback);

        if (!VK::initCore()) {
            std::cerr << "Failed to initialize Vulkan core!\n";
            return;
        }

        renderThreadContext = new ContextWrapper(VK::createRenderThreadContext());
        if (!renderThreadContext->context) {
            std::cerr << "Render thread Vulkan context was not created!\n";
            return;
        }

        glfwGetFramebufferSize(window, &width, &height);
        currentWidth = width;
        currentHeight = height;

        if (!VK::initWindowSurface(window, width, height, currentVsync)) {
            std::cerr << "Vulkan swapchain was not created!\n";
            return;
        }

        rebuildSwapchainSurfaces();
        if (swapchainSurfaces.empty() || !swapchainSurfaces[0]->surface) {
            std::cerr << "Swapchain surfaces were not created!\n";
            return;
        }

        ready = true;

        while (!glfwWindowShouldClose(window)) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwPollEvents();

            // Skip rendering while window is minimized (0x0 framebuffer)
            if (width == 0 || height == 0) {
                continue;
            }

            if (pendingResize) {
                pendingResize = false;
                width = pendingW;
                height = pendingH;
                renderThreadContext->context->flushAndSubmit(GrSyncCpu::kYes);
                clearSwapchainSurfaces();
                renderThreadContext->context->freeGpuResources();
                VK::recreateSwapchain(pendingW, pendingH, currentVsync);
                rebuildSwapchainSurfaces();

                wPropertiesViewer[0] = true;
                wPropertiesViewer[1] = pendingW;
                wPropertiesViewer[2] = pendingH;
            }

            if (!vsyncQueue.empty()) {
                currentVsync = vsyncQueue.back();
                vsyncQueue.clear();
                renderThreadContext->context->flushAndSubmit(GrSyncCpu::kYes);
                clearSwapchainSurfaces();
                VK::recreateSwapchain(width, height, currentVsync);
                rebuildSwapchainSurfaces();
            }

            uint32_t imageIndex = 0;
            if (!VK::acquireNextImage(&imageIndex)) {
                // Swapchain became out-of-date; recreate and retry next frame
                clearSwapchainSurfaces();
                VK::recreateSwapchain(width, height, currentVsync);
                rebuildSwapchainSurfaces();
                continue;
            }

            canvas = swapchainSurfaces[imageIndex]->surface->getCanvas();
            canvas->clear(SK_ColorTRANSPARENT);

            render_composited_canvases(canvas);

            onrefresh(0);

            VK::presentImage(
                renderThreadContext->context.get(),
                swapchainSurfaces[imageIndex]->surface.get(),
                imageIndex
            );

            frameReady = false;
        }

        ready = false;
        VK::waitIdle();
        clearSwapchainSurfaces();
        delete renderThreadContext;
        renderThreadContext = nullptr;
        glfwDestroyWindow(window);
        VK::shutdown();
        glfwTerminate();
    }

    WINDOWS_EXPORT bool canvas_init_gpu_context() {
        if (!VK::initCore()) return false;
        mainThreadCtx = new ContextWrapper(VK::createMainThreadContext());
        gpuContextReady = (mainThreadCtx && mainThreadCtx->context);

        if (gpuContextReady) {
            std::cout << "[OK] canvas_init_gpu_context()\n";
        } else {
            std::cerr << "[ERR] canvas_init_gpu_context() failed!\n";
        }
        return gpuContextReady;
    }

    WINDOWS_EXPORT void canvas_flush_gpu() {
        if (!gpuContextReady) return;
        VK::flushAndSyncMainContext(mainThreadCtx->context.get());
    }
    #else
    WINDOWS_EXPORT void setup_render_thread(int w, int h, const char* title, JSCallback_WRefresh onrefresh, bool vsync) {
        width = w;
        height = h;
        currentVsync = vsync;

        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            std::cerr << "Couldn't initialize Window...\n";
            const char* desc;
            int code = glfwGetError(&desc);
            std::cerr << "GLFW error " << code << ": " << (desc ? desc : "unknown") << "\n";
            glfwTerminate();
            return;
        }

        glfwSetWindowSizeCallback(window, window_resize_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window, keyboard_key_callback);
        glfwSetWindowRefreshCallback(window, window_refresh_callback);

        if (!VK::initCore()) return;
        renderThreadContext = new ContextWrapper(VK::createRenderThreadContext());
        if (!renderThreadContext->context) return;

        glfwGetFramebufferSize(window, &width, &height);
        currentWidth = width;
        currentHeight = height;

        if (!VK::initWindowSurface(window, width, height, currentVsync)) return;
        rebuildSwapchainSurfaces();
        if (swapchainSurfaces.empty()) return;

        ready = true;
    }

    WINDOWS_EXPORT bool canvas_init_gpu_context() { return false; }
    WINDOWS_EXPORT void canvas_flush_gpu() {}
    #endif

    WINDOWS_EXPORT void update_window() {
    #ifdef __APPLE__
        wPropertiesViewer[3] = glfwWindowShouldClose(window);
        if (glfwWindowShouldClose(window)) return;
        glfwGetFramebufferSize(window, &width, &height);
        glfwPollEvents();
        if (width == 0 || height == 0) return;

        if (pendingResize) {
            pendingResize = false;
            width = pendingW;
            height = pendingH;
            clearSwapchainSurfaces();
            renderThreadContext->context->freeGpuResources();
            VK::recreateSwapchain(pendingW, pendingH, currentVsync);
            rebuildSwapchainSurfaces();
            wPropertiesViewer[0] = true;
            wPropertiesViewer[1] = pendingW;
            wPropertiesViewer[2] = pendingH;
        }

        uint32_t imageIndex = 0;
        if (!VK::acquireNextImage(&imageIndex)) return;

        canvas = swapchainSurfaces[imageIndex]->surface->getCanvas();
        canvas->clear(SK_ColorTRANSPARENT);
        render_composited_canvases(canvas);

        VK::presentImage(
            renderThreadContext->context.get(),
            swapchainSurfaces[imageIndex]->surface.get(),
            imageIndex
        );
    #endif
    }

    WINDOWS_EXPORT void destroy_window() {
    #ifdef __APPLE__
        ready = false;
        VK::waitIdle();
        clearSwapchainSurfaces();
        delete renderThreadContext;
        renderThreadContext = nullptr;
        glfwDestroyWindow(window);
        VK::shutdown();
        glfwTerminate();
    #endif
    }
}