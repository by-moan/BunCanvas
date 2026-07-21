bool pollingEvents = false;



bool shouldClose = false;
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
    mMoveViewer[3] = xpos-prevX;
    mMoveViewer[4] = ypos-prevY;
    prevX = xpos;
    prevY = ypos;
}

bool clickStarted = 0;
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

void window_refresh_callback(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    canvas->clear(SK_ColorTRANSPARENT);
    
    for (auto element : canvases) {
        {
            // std::lock_guard<std::mutex> lock(element->mutex);
            canvas->drawImage(element->surface->makeTemporaryImage(),0,0);
        }
    }
    renderThreadContext->context->flushAndSubmit(GrSyncCpu::kYes);
    glfwSwapBuffers(window);
}

typedef int (*JSCallback_WRefresh)(int);
typedef int (*JSCallback_WClose)();


extern "C" {
    WINDOWS_EXPORT void create_window(int w, int h, const char* title){
    }
    
    #ifndef __APPLE__
    // Called in the bun worker thread.
    WINDOWS_EXPORT void setup_render_thread(int w, int h, const char* title, JSCallback_WRefresh onrefresh, bool vsync){
        if (!glfwInit()) {
            std::cerr << "Couldn't initialize GLFW...\n";
            return;
        };
        width = w;
        height = h;
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            std::cerr << "Couldn't initialize Window...\n";
            const char* desc;
            int code = glfwGetError(&desc);
            std::cerr << "GLFW error " << code << ": "
            << (desc ? desc : "unknown")
            << "\n";
            glfwTerminate();
            return;
        }
        
        sharedWindow = glfwCreateWindow(1, 1, "shared", NULL, window);
        if (sharedWindow) {
            glfwHideWindow(sharedWindow);
            onrefresh(1);
        }
        
        #pragma region Events Setup
        
        glfwSetWindowSizeCallback(window, window_resize_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window,keyboard_key_callback);
        // glfwSetWindowRefreshCallback(window, window_refresh_callback);
        #pragma endregion
        
        glfwMakeContextCurrent(window);
        renderThreadInterface = new InterfaceWrapper(GrGLMakeNativeInterface());
        if (!renderThreadInterface->interface) {
            std::cout << "Native interface failed\n";
            return;
        }
        
        std::cout << "Native interface OK\n";
        
        renderThreadContext = new ContextWrapper(GrDirectContexts::MakeGL(renderThreadInterface->interface));
        if (!renderThreadContext->context) {
            std::cout << "Context was not created!\n";
            return;
        };
        
        GrGLFramebufferInfo fbInfo;
        fbInfo.fFBOID = 0;
        fbInfo.fFormat = GL_RGBA8;
        
        
        glfwGetFramebufferSize(window, &width, &height);
        
        currentWidth = width;
        currentHeight = height;
        
        sWrapper = new SurfaceWrapper(createSurface(
            renderThreadContext->context.get(),
            width,
            height
        ));
        
        if (!sWrapper->surface) {
            std::cout << "Surface was not created!\n";
            return;
        };
        
        canvas = sWrapper->surface->getCanvas();
        
        clearColor.setColor(SK_ColorTRANSPARENT);
        clearColor.setStyle(SkPaint::kFill_Style);
        clearColor.setBlendMode(SkBlendMode::kClear);
        clearColor.setAntiAlias(1);
        pImageDataColor.setBlendMode(SkBlendMode::kSrc);
        
        ready = true;
        glfwSwapInterval(vsync);
        while (!glfwWindowShouldClose(window)) {
            // loop_mutex.lock();
            
            glfwGetFramebufferSize(window, &width, &height);
            
            glfwPollEvents();
            
            
            canvas->clear(SK_ColorTRANSPARENT);
            
            if (pendingResize == true){
                pendingResize = false;
                glViewport(0, 0, pendingW, pendingH);
                width = pendingW;
                height = pendingH;
                sWrapper->surface.reset();
                sWrapper->surface = createSurface(
                    renderThreadContext->context.get(),
                    pendingW,
                    pendingH
                );
                canvas = sWrapper->surface->getCanvas();
                // renderThreadContext->context->flushAndSubmit(GrSyncCpu::kYes);
                wResizeViewer[0] = true;
                wResizeViewer[1] = pendingW;
                wResizeViewer[2] = pendingH;
            }
            
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
                        canvas->drawImage(img, 0, 0);
                    }
                } else {
                    canvas->drawImage(element->surface->makeTemporaryImage(), 0, 0);
                }
                
                // Clean up retired textures after the frame delay
                // ensures the render thread has finished using them
                auto& retired = element->retiredTextures;
                for (auto it = retired.begin(); it != retired.end(); ) {
                    it->framesLeft--;
                    if (it->framesLeft <= 0) {
                        GrGLTextureInfo info;
                        if (GrBackendTextures::GetGLTextureInfo(it->tex, &info)) {
                            glDeleteTextures(1, &info.fID);
                        }
                        it = retired.erase(it);
                    } else {
                        ++it;
                    }
                }

                #else
                canvas->drawImage(element->surface->makeTemporaryImage(), 0, 0);
                #endif
            }
            onrefresh(0);
            renderThreadContext->context->flushAndSubmit(GrSyncCpu::kYes);
            
            glfwSwapBuffers(window);
            std::unique_lock<std::mutex> lock(frameMtx);
            frameCv.wait(lock, [] { return frameReady; });
            frameReady = false;
            lock.unlock();

            // loop_mutex.unlock();
        }
        ready = false;
        shouldClose = true;
        glfwDestroyWindow(window);
        glfwTerminate();
        delete renderThreadInterface;
    }
    
    WINDOWS_EXPORT bool canvas_init_gpu_context() {
        if (!sharedWindow) return false;
        glfwMakeContextCurrent(sharedWindow);
        mainThreadIface = new InterfaceWrapper(GrGLMakeNativeInterface());
        if (!mainThreadIface->interface) return false;
        mainThreadCtx = new ContextWrapper(
            GrDirectContexts::MakeGL(mainThreadIface->interface)
        );
        gpuContextReady = (mainThreadCtx && mainThreadCtx->context);
        return gpuContextReady;
    }
    
    WINDOWS_EXPORT void canvas_flush_gpu() {
        if (!gpuContextReady) return;
        // Block the JS thread until the GPU finishes drawing.
        // This guarantees the render thread can read the textures safely.
        mainThreadCtx->context->flushAndSubmit(GrSyncCpu::kYes);
    }
    #else
    WINDOWS_EXPORT void setup_render_thread(int w, int h, const char* title, JSCallback_WRefresh onrefresh, bool vsync){
        if (!glfwInit()) {
            std::cerr << "Couldn't initialize GLFW...\n";
            return;
        };
        width = w;
        height = h;
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!window) {
            std::cerr << "Couldn't initialize Window...\n";
            const char* desc;
            int code = glfwGetError(&desc);
            std::cerr << "GLFW error " << code << ": "
            << (desc ? desc : "unknown")
            << "\n";
            glfwTerminate();
            return;
        }
        
        #pragma region Events Setup
        
        glfwSetWindowSizeCallback(window, window_resize_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window,keyboard_key_callback);
        glfwSetWindowRefreshCallback(window, window_refresh_callback);
        #pragma endregion
        
        glfwMakeContextCurrent(window);
        renderThreadInterface = new InterfaceWrapper(GrGLMakeNativeInterface());
        if (!renderThreadInterface->interface) {
            std::cout << "Native interface failed\n";
            return;
        }
        
        std::cout << "Native interface OK\n";
        
        renderThreadContext = new ContextWrapper(GrDirectContexts::MakeGL(renderThreadInterface->interface));
        if (!renderThreadContext->context) {
            std::cout << "Context was not created!\n";
            return;
        };
        
        GrGLFramebufferInfo fbInfo;
        fbInfo.fFBOID = 0;
        fbInfo.fFormat = GL_RGBA8;
        
        
        glfwGetFramebufferSize(window, &width, &height);
        
        currentWidth = width;
        currentHeight = height;
        
        sWrapper = new SurfaceWrapper(createSurface(
            renderThreadContext->context.get(),
            width,
            height
        ));
        
        if (!sWrapper->surface) {
            std::cout << "Surface was not created!\n";
            return;
        };
        std::cout << vsync << "\n";
        glfwSwapInterval(vsync);
        
        canvas = sWrapper->surface->getCanvas();
        
        clearColor.setColor(SK_ColorTRANSPARENT);
        clearColor.setStyle(SkPaint::kFill_Style);
        clearColor.setBlendMode(SkBlendMode::kClear);
        clearColor.setAntiAlias(1);
        pImageDataColor.setBlendMode(SkBlendMode::kSrc);

        ready = true;
    }
    
    WINDOWS_EXPORT bool canvas_init_gpu_context() {}
    WINDOWS_EXPORT void canvas_flush_gpu() {}
    #endif
    
    #ifndef __APPLE__
    WINDOWS_EXPORT void update_window( JSCallback_WRefresh onrefresh){}
    #else
    WINDOWS_EXPORT void update_window(JSCallback_WRefresh onrefresh){
        glfwGetFramebufferSize(window, &width, &height);
        
        glfwPollEvents();
        canvas->clear(SK_ColorTRANSPARENT);
        
        if (pendingResize == true){
            pendingResize = false;
            glViewport(0, 0, pendingW, pendingH);
            width = pendingW;
            height = pendingH;
            sWrapper->surface.reset();
            sWrapper->surface = createSurface(
                renderThreadContext->context.get(),
                pendingW,
                pendingH
            );
            canvas = sWrapper->surface->getCanvas();
            // renderThreadContext->context->flushAndSubmit(GrSyncCpu::kYes);
            wResizeViewer[0] = true;
            wResizeViewer[1] = pendingW;
            wResizeViewer[2] = pendingH;
        }
        
        // processResizes();
        onrefresh(1);
        
        for (auto element : canvases) {
            canvas->drawImage(element->surface->makeTemporaryImage(),0,0);
        }
        renderThreadContext->context->flushAndSubmit(GrSyncCpu::kYes);
        glfwSwapBuffers(window);
    }
    #endif
    
    
    
    
    
    
    #ifndef __APPLE__
    WINDOWS_EXPORT bool should_window_close() {
        return shouldClose;
    }
    WINDOWS_EXPORT void destroy_window() {
        // delete ptr;
    }
    #else
    WINDOWS_EXPORT bool should_window_close() {
        return glfwWindowShouldClose(window);
    }
    WINDOWS_EXPORT void destroy_window() {
        ready = false;
        glfwDestroyWindow(window);
        glfwTerminate();
        delete renderThreadInterface;
    }
    
    #endif
}