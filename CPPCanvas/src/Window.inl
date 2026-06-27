bool pollingEvents = false;

int32_t* wResizeViewer = nullptr;
_Float64_t* mMoveViewer = nullptr;
_Float64_t* mClickViewer = nullptr;
_Float64_t* mDownViewer = nullptr;
_Float64_t* mUpViewer = nullptr;

int32_t* kDownViewer = nullptr;
int32_t* kUpViewer = nullptr;

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
    prevX = xpos;
    prevY = ypos;
}
    
bool clickStarted = 0;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (pollingEvents == true || mClickViewer == nullptr || mMoveViewer == nullptr) return;
    
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

extern "C" {
    void create_window(int w, int h, const char* title,
        int32_t* wRViewer,
        _Float64_t* mMViewer,
        _Float64_t* mCViewer,
        _Float64_t* mDViewer, 
        _Float64_t* mUViewer,
        int32_t* kDViewer,
        int32_t* kUViewer
    ){
        if (!glfwInit()) {
            std::cerr << "Couldn't initialize GLFW...\n";
            return;
        };

        wResizeViewer = wRViewer;
        mMoveViewer = mMViewer;
        mClickViewer = mCViewer;
        mDownViewer = mDViewer;
        mUpViewer = mUViewer;

        kDownViewer = kDViewer;
        kUpViewer = kUViewer;
        
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
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window,keyboard_key_callback);
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
    
    void update_window(
        int32_t* wRViewer,
        _Float64_t* mMViewer,
        _Float64_t* mCViewer,
        _Float64_t* mDViewer,
        _Float64_t* mUViewer,
        int32_t* kDViewer,
        int32_t* kUViewer
    ){
        glfwPollEvents();
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        if (wRViewer != wResizeViewer) wResizeViewer = wRViewer;
        if (mMViewer != mMoveViewer) mMoveViewer = mMViewer;
        if (mCViewer != mClickViewer) mClickViewer = mCViewer; 
        if (mDViewer != mDownViewer) mDownViewer = mDViewer; 
        if (mUViewer != mUpViewer) mUpViewer = mUViewer; 

        if (kDViewer != kDownViewer) kDownViewer = kDViewer; 
        if (kUViewer != kUpViewer) kUpViewer = kUViewer; 

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