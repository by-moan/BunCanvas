extern "C" {
    void create_window(int w, int h, const char* title = "App"){
        if (!glfwInit()) {
            std::cerr << "Couldn't initialize GLFW...\n";
            return;
        };
        
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
    }
    
    void update_window(){
        glfwPollEvents();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        if (width != currentWidth || height != currentHeight) {
            currentWidth = width;
            currentHeight = height;
            
            glViewport(0, 0, width, height);
            
            ctxWrapper->context->flush();
            sWrapper->surface.reset();
            sWrapper->surface = createSurface(
                ctxWrapper->context.get(),
                width,
                height
            );
            canvas = sWrapper->surface->getCanvas();
        }

        for (auto element : canvases) {
            canvas->drawImage(element->surface->makeImageSnapshot(),0,0);
        }
        
        // canvas->clear(SK_ColorTRANSPARENT);
        
        // SkPaint p;
        // p.setColor(SK_ColorRED);
        // p.setAntiAlias(true);
        
        // canvas->drawRect(
        //     SkRect::MakeXYWH(400, 200, 400, 400),
        //     p
        // );
        
        ctxWrapper->context->flush();
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