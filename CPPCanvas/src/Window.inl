bool pollingEvents = false;

int32_t* wResizeViewer = nullptr;
_Float64_t* mMoveViewer = nullptr;
_Float64_t* mClickViewer = nullptr;

void window_resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
            
    ctxWrapper->context->flush();
    sWrapper->surface.reset();
    sWrapper->surface = createSurface(
        ctxWrapper->context.get(),
        width,
        height
    );
    canvas = sWrapper->surface->getCanvas();
    if (pollingEvents == true || wResizeViewer == nullptr) return;
    wResizeViewer[0] = true;
    wResizeViewer[1] = width;
    wResizeViewer[2] = height;
}

double prevX = 0;
double prevY = 0;

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (pollingEvents == true || mMoveViewer == nullptr || xpos == prevX || ypos == prevY) return;
    mMoveViewer[0] = true;
    mMoveViewer[1] = xpos;
    mMoveViewer[2] = ypos;
    mMoveViewer[3] = xpos-prevX;
    mMoveViewer[4] = ypos-prevY;
    // events.emplace_back("mmove", "{\"xpos\":" + std::to_string(xpos) + ", \"ypos\": " + std::to_string(ypos) + ",\"movx\":" + std::to_string(xpos-prevX) + ",\"movy\":" + std::to_string(ypos-prevY) + "}");
    prevX = xpos;
    prevY = ypos;
}
    
int buttons = 0;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (pollingEvents == true || mClickViewer == nullptr || mMoveViewer == nullptr) return;
    mClickViewer[0] = true;
    mClickViewer[1] = mMoveViewer[1];
    mClickViewer[2] = mMoveViewer[2];
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mClickViewer[3] = 0;
        mClickViewer[4]+=(double)0b1;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        mClickViewer[3] = 1;
        mClickViewer[4]+=(double)0b10;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        mClickViewer[3] = 2;
        mClickViewer[4]+=(double)0b100;
    }
    // events.emplace_back("mmove", "{\"xpos\":" + std::to_string(currentX) + ", \"ypos\": " + std::to_string(currentY) + ",\"button\":" + std::to_string(button) + ",\"buttons\":" + std::to_string(buttons) + "}");

}



extern "C" {
    void create_window(int w, int h, const char* title, int32_t* wRViewer, _Float64_t* mMViewer, _Float64_t* mCViewer){
        if (!glfwInit()) {
            std::cerr << "Couldn't initialize GLFW...\n";
            return;
        };

        wResizeViewer = wRViewer;
        mMoveViewer = mMViewer;
        mClickViewer = mCViewer;
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        window = glfwCreateWindow(w, h, title, NULL, NULL);
        if (!window) {
            std::cerr << "Couldn't initialize Window...\n";
            glfwTerminate();
            return;
        }
        
        #pragma region Events Setup
        
        glfwSetWindowSizeCallback(window, window_resize_callback);
        glfwSetCursorPosCallback(window, cursor_pos_callback);
        #pragma endregion
        
        glfwMakeContextCurrent(window);
        
        auto iface = GrGLMakeNativeInterface();
        
        if (!iface) {
            std::cout << "Native interface failed\n";
            return;
        }
        
        std::cout << "Native interface OK\n";
        
        ctxWrapper = new ContextWrapper(GrDirectContexts::MakeGL(iface));
        if (!ctxWrapper->context) {
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
            ctxWrapper->context.get(),
            width,
            height
        ));
        
        if (!sWrapper->surface) {
            std::cout << "Surface was not created!\n";
            return;
        };
        
        glfwGetFramebufferSize(window, &width, &height);
        
        canvas = sWrapper->surface->getCanvas();

        clearColor.setColor(SK_ColorTRANSPARENT);
        clearColor.setStyle(SkPaint::kFill_Style);
        clearColor.setBlendMode(SkBlendMode::kClear);
        clearColor.setAntiAlias(1);
    }
    
    void update_window(int32_t* wRViewer, _Float64_t* mMViewer, _Float64_t* mCViewer){
        glfwPollEvents();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        // if (width != currentWidth || height != currentHeight) {
        //     currentWidth = width;
        //     currentHeight = height;
            
        //     glViewport(0, 0, width, height);
            
        //     ctxWrapper->context->flush();
        //     sWrapper->surface.reset();
        //     sWrapper->surface = createSurface(
        //         ctxWrapper->context.get(),
        //         width,
        //         height
        //     );
        //     canvas = sWrapper->surface->getCanvas();
        // }
        if (wRViewer != wResizeViewer) wResizeViewer = wRViewer;
        if (mMViewer != mMoveViewer) mMoveViewer = mMViewer;
        if (mCViewer != mClickViewer) mClickViewer = mCViewer; 

        canvas->clear(SK_ColorTRANSPARENT);

        
        for (auto element : canvases) {
            // if (element->locked == true) continue;
            // canvas->drawImage(element->surface->makeImageSnapshot(),0,0);
            canvas->drawImage(element->surface->makeTemporaryImage(),0,0);
        }
        
        
        // SkPaint p;
        // p.setColor(SK_ColorRED);
        // p.setAntiAlias(true);
        
        // canvas->drawRect(
        //     SkRect::MakeXYWH(400, 200, 400, 400),
        //     p
        // );
        
        ctxWrapper->context->flushAndSubmit();
        glfwSwapBuffers(window);
    }
    
    
    bool should_window_close() {
        return glfwWindowShouldClose(window);
    }
    
    void destroy_window() {
        glfwDestroyWindow(window);
        glfwTerminate();
        // delete ptr;
    }
}