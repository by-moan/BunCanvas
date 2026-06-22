class Event {
    public:
    std::string evtType;
    std::string values;
    
    // This event is data-oriented, mainly so the Bun engine can parse it and use it internally
    Event(std::string type, std::string val) : evtType(type), values(val) {}
};

std::vector<Event> events;
std::string events_buffer;
bool pollingEvents = false;

void window_resize_callback(GLFWwindow* window, int width, int height) {
    if (pollingEvents == true) return;
    events.emplace_back("wresize", "{\"width\":" + std::to_string(width) + ", \"height\": " + std::to_string(height) + "}");
}

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
        
        #pragma region Events Setup
        
        glfwSetWindowSizeCallback(window, window_resize_callback);
        
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
    }
    
    const char* window_query_events() {
        if (pollingEvents == true) return "[]";

        pollingEvents = true;
        events_buffer = "[";
        
        
        if (events.empty() == false) {
            for (auto item : events) {
                events_buffer+="{\"type\":\"" + item.evtType + "\",\"values\":" + item.values + "},";
            }
            events_buffer.pop_back();
        }
        events_buffer += "]";
        pollingEvents = false;
        

        events.clear();

        return events_buffer.c_str();
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
            if (element->locked == true) continue;
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