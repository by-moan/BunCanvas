bool pollingEvents = false;

int32_t* wResizeViewer = nullptr;
_Float64_t* mMoveViewer = nullptr;
_Float64_t* mClickViewer = nullptr;
_Float64_t* mDownViewer = nullptr;
_Float64_t* mUpViewer = nullptr;

int32_t* kDownViewer = nullptr;
int32_t* kUpViewer = nullptr;




void processCommands() {
    if(ready == false) return;
    std::queue<Command> local;
    
    {
        std::lock_guard lock(queueMutex);
        std::swap(local, cmdQueue);
    }
    
    while (!local.empty()) {
        Command& cmd = local.front();
        
        switch (cmd.type) {
            case CommandType::canvas_set_fill_style : {
                try {
                    cmd.styleCmd.color->setColor(keywordColors.at(cmd.styleCmd.c));
                    break;
                }catch (std::exception){
                    const char* c = cmd.styleCmd.c;
                    if (c[0] == '#') {
                        unsigned int rgb = std::strtoul(c+1, nullptr, 16);
                        cmd.styleCmd.color->setColor(SkColor4f{
                            ((rgb >> 16) & 0xFF) / 255.f,
                            ((rgb >> 8) & 0xFF) / 255.f,
                            (rgb & 0xFF) / 255.f,
                            1.0f
                        });
                        break;
                    }\
                    if (c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == '(') {
                        c+=4;
                        int r = 0;
                        while (*c >= '0' && *c <= '9') {
                            r = r * 10 + (*c - '0');
                            ++c;
                        }
                        r = std::max(r,0);
                        r = std::min(r,255);
                        while (*c == ' ' || *c == ',') ++c;
                        int g = 0;
                        while (*c >= '0' && *c <= '9') {
                            g = g * 10 + (*c - '0');
                            ++c;
                        }
                        g = std::max(g,0);
                        g = std::min(g,255);
                        while (*c == ' ' || *c == ',') ++c;
                        int b = 0;
                        while (*c >= '0' && *c <= '9') {
                            b = b * 10 + (*c - '0');
                            ++c;
                        }
                        b = std::max(b,0);
                        b = std::min(b,255);
                        cmd.styleCmd.color->setColor(SkColor4f{
                            (float)r / 255.f,
                            (float)g / 255.f,
                            (float)b / 255.f,
                            1.0f
                        });
                        break;
                    }
                }
                break;
            }
            case CommandType::canvas_set_stroke_width : {
                cmd.strokeWidthCmd.renderingContext->strokeColor.setStrokeWidth(cmd.strokeWidthCmd.w);
                break;
            }
            case CommandType::canvas_fill_rect : {
                (*cmd.rectCmd.renderingContext)()->drawRect(
                    SkRect::MakeXYWH(
                        cmd.rectCmd.x,
                        cmd.rectCmd.y,
                        cmd.rectCmd.w,
                        cmd.rectCmd.h
                    ),
                    cmd.rectCmd.renderingContext->fillColor
                );
                break;
            }
            
            case CommandType::canvas_clear_rect : {
                (*cmd.rectCmd.renderingContext)()->drawRect(
                    SkRect::MakeXYWH(
                        cmd.rectCmd.x,
                        cmd.rectCmd.y,
                        cmd.rectCmd.w,
                        cmd.rectCmd.h
                    ),
                    clearColor
                );
                break;
            }
            case CommandType::canvas_resize : {
                cmd.resizeCmd.canvas->resize(cmd.resizeCmd.w,cmd.resizeCmd.h);
                break;
            }
            case CommandType::canvas_path_begin : {
                cmd.renderContextOnlyCmd.renderingContext->pathBuilder.reset();
                break;
            }
            case CommandType::canvas_path_move_to : {
                cmd.rectCmd.renderingContext->pathBuilder.moveTo(cmd.rectCmd.x,cmd.rectCmd.y);
                break;
            }
            case CommandType::canvas_path_line_to : {
                cmd.rectCmd.renderingContext->pathBuilder.lineTo(cmd.rectCmd.x,cmd.rectCmd.y);
                break;
            }
            case CommandType::canvas_path_arc : {
                cmd.sixFloatCmd.renderingContext->pathBuilder.addArc(SkRect::MakeXYWH(cmd.sixFloatCmd.f1 - cmd.sixFloatCmd.f3,cmd.sixFloatCmd.f2 - cmd.sixFloatCmd.f3,cmd.sixFloatCmd.f3 * 2,cmd.sixFloatCmd.f3 * 2), cmd.sixFloatCmd.f4*57.29577958f, cmd.sixFloatCmd.f5*57.29577958f);
                break;
            }
            case CommandType::canvas_path_arc_to : {
                cmd.sixFloatCmd.renderingContext->pathBuilder.arcTo({cmd.sixFloatCmd.f1,cmd.sixFloatCmd.f2},{cmd.sixFloatCmd.f3,cmd.sixFloatCmd.f4},cmd.sixFloatCmd.f5);
                break;
            }
            case CommandType::canvas_path_bezier_to : {
                cmd.sixFloatCmd.renderingContext->pathBuilder.cubicTo(cmd.sixFloatCmd.f1,cmd.sixFloatCmd.f2,cmd.sixFloatCmd.f3,cmd.sixFloatCmd.f4,cmd.sixFloatCmd.f5,cmd.sixFloatCmd.f6);
                break;
            }
            case CommandType::canvas_path_stroke : {
                (*cmd.renderContextOnlyCmd.renderingContext)()->drawPath(cmd.renderContextOnlyCmd.renderingContext->pathBuilder.snapshot(),cmd.renderContextOnlyCmd.renderingContext->strokeColor);
                break;
            }
            case CommandType::canvas_draw_image : {
                (*cmd.imageWrapperRectCmd.renderingContext)()->drawImageRect(cmd.imageWrapperRectCmd.Image->image.get(),SkRect::MakeXYWH(cmd.imageWrapperRectCmd.x,cmd.imageWrapperRectCmd.y,cmd.imageWrapperRectCmd.w,cmd.imageWrapperRectCmd.h),cmd.imageWrapperRectCmd.renderingContext->sampling,&(cmd.imageWrapperRectCmd.renderingContext->imageColor));
                break;
            }
            case CommandType::canvas_get_image_data : {
                SkImageInfo dstInfo = SkImageInfo::Make(
                    cmd.rectBufferPtrCmd.w,cmd.rectBufferPtrCmd.h,
                    kRGBA_8888_SkColorType,
                    kPremul_SkAlphaType
                );
                size_t rowBytes = cmd.rectBufferPtrCmd.w * 4;
            
                auto sfc = (*cmd.rectBufferPtrCmd.renderingContext)()->getSurface();
                if (sfc == nullptr) return;
                auto tmp = sfc->makeTemporaryImage();
                tmp->readPixels(
                    dstInfo, 
                    cmd.rectBufferPtrCmd.ptr, 
                    rowBytes, 
                    cmd.rectBufferPtrCmd.x, 
                    cmd.rectBufferPtrCmd.y, 
                    SkImage::CachingHint::kDisallow_CachingHint
                );
                break;
            }
            case CommandType::canvas_put_image_data : {
                SkImageInfo imageInfo = SkImageInfo::Make(
                    cmd.rectBufferPtrCmd.w,cmd.rectBufferPtrCmd.h,
                    kRGBA_8888_SkColorType,
                    kPremul_SkAlphaType
                );
                
                SkBitmap bitmap;
                
                if(bitmap.installPixels(imageInfo,cmd.rectBufferPtrCmd.ptr,cmd.rectBufferPtrCmd.w *4)){
                    (*cmd.rectBufferPtrCmd.renderingContext)()->drawImage(bitmap.asImage(),cmd.rectBufferPtrCmd.x,cmd.rectBufferPtrCmd.y);
                    break;
                }
                break;
            }
            case CommandType::canvas_set_composite_operation : {
                try {
                    cmd.compositeOperationCmd.renderingContext->fillColor.setBlendMode(compositeOperations.at(cmd.compositeOperationCmd.operation));
                    cmd.compositeOperationCmd.renderingContext->strokeColor.setBlendMode(compositeOperations.at(cmd.compositeOperationCmd.operation));
                    cmd.compositeOperationCmd.renderingContext->imageColor.setBlendMode(compositeOperations.at(cmd.compositeOperationCmd.operation));
                    break;
                }catch(std::exception err) {
                    break;
                }
                break;
            }
        }
        
        local.pop();
    }
}


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
    if (pollingEvents == true || mMoveViewer == nullptr) return;
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

void window_refresh_callback(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    canvas->clear(SK_ColorTRANSPARENT);
    
    
    for (auto element : canvases) {
        canvas->drawImage(element->surface->makeTemporaryImage(),0,0);
    }
    ctxWrapper->context->flushAndSubmit();
    glfwSwapBuffers(window);
}
std::string wTitle;
bool windowDestroyed = false;

extern "C" {
    WINDOWS_EXPORT void create_window(int w, int h, const char* title,
        int32_t* wRViewer,
        _Float64_t* mMViewer,
        _Float64_t* mCViewer,
        _Float64_t* mDViewer, 
        _Float64_t* mUViewer,
        int32_t* kDViewer,
        int32_t* kUViewer
    ){
        width = w;
        height = h;
        
        wTitle = title;
        wResizeViewer = wRViewer;
        mMoveViewer = mMViewer;
        mClickViewer = mCViewer;
        mDownViewer = mDViewer;
        mUpViewer = mUViewer;
        
        // wResizeViewer[0] = w;
        // wResizeViewer[1] = h;
        
        kDownViewer = kDViewer;
        kUpViewer = kUViewer;
        
        
        
        clearColor.setColor(SK_ColorTRANSPARENT);
        clearColor.setStyle(SkPaint::kFill_Style);
        clearColor.setBlendMode(SkBlendMode::kClear);
        clearColor.setAntiAlias(1);
        
        // renderT = new std::thread(InitRenderThread);
        // #ifdef _WIN64
        // glfwSwapInterval(0);
        // #endif
    }
    
    void setup_render_thread(){
        std::cout << "Checkpoint before!\n";
        if (!glfwInit()) {
            std::cerr << "Couldn't initialize GLFW...\n";
            return;
        };
        std::cout << "Checkpoint after!\n";
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_STENCIL_BITS, 8);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        window = glfwCreateWindow(width, height, "App", NULL, NULL);
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
        // glfwSetWindowRefreshCallback(window, window_refresh_callback);
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
        
        canvas = sWrapper->surface->getCanvas();
        
        ready = true;
        while (!glfwWindowShouldClose(window))
        {
            // int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            canvas->clear(SK_ColorTRANSPARENT);
            
            processCommands();
            for (auto element : canvases) {
                canvas->drawImage(element->surface->makeTemporaryImage(),0,0);
            }
            ctxWrapper->context->flushAndSubmit();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        ready = false;
        glfwDestroyWindow(window);
        glfwTerminate();
        windowDestroyed = true;
    }
    
    
    WINDOWS_EXPORT void update_window(
        int32_t* wRViewer,
        _Float64_t* mMViewer,
        _Float64_t* mCViewer,
        _Float64_t* mDViewer,
        _Float64_t* mUViewer,
        int32_t* kDViewer,
        int32_t* kUViewer
    ){
        if (wRViewer != wResizeViewer) wResizeViewer = wRViewer;
        if (mMViewer != mMoveViewer) mMoveViewer = mMViewer;
        if (mCViewer != mClickViewer) mClickViewer = mCViewer; 
        if (mDViewer != mDownViewer) mDownViewer = mDViewer; 
        if (mUViewer != mUpViewer) mUpViewer = mUViewer; 
        
        if (kDViewer != kDownViewer) kDownViewer = kDViewer; 
        if (kUViewer != kUpViewer) kUpViewer = kUViewer; 
        
        
        // #ifdef _WIN64
        // GLFWmonitor* monitor = glfwGetWindowMonitor(window);
        // if (!monitor)
        //     monitor = glfwGetPrimaryMonitor();
        
        // const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        
        // int refreshRate = mode->refreshRate;
        // std::cout << refreshRate << "\n";
        // std::this_thread::sleep_for(
        //     std::chrono::microseconds(
        //         static_cast<int>(1000000.0 / refreshRate)
        //     )
        // );
        // #endif
    }
    
    
    WINDOWS_EXPORT bool should_window_close() {
        return windowDestroyed;
    }
    
    WINDOWS_EXPORT void destroy_window() {
        // delete renderT;
        // delete ptr;
    }
}