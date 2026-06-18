#include <iostream>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg.h"
#include "nanovg_gl.h"

GLFWwindow* window = nullptr;
NVGcontext* vg = nullptr;

bool window_destroyed = false;


namespace CanvasProperties {
	NVGcolor fillStyle = nvgRGB(0,0,0);
	NVGcolor strokeStyle = nvgRGB(0,0,0);
	int winWidth, winHeight;
	int fbWidth, fbHeight;
};


void draw_prepare(){
	glfwGetWindowSize(window, &CanvasProperties::winWidth, &CanvasProperties::winHeight);
	glfwGetFramebufferSize(window, &CanvasProperties::fbWidth, &CanvasProperties::fbHeight);
	glViewport(0, 0, CanvasProperties::fbWidth, CanvasProperties::fbHeight);
	
	
	// Clear the background
	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	float pxRatio = (float)CanvasProperties::fbWidth / (float)CanvasProperties::winWidth;
	// --- NanoVG Drawing Starts Here ---
	// nvgBeginFrame(vg, CanvasProperties::fbWidth,CanvasProperties::fbHeight, pxRatio);
	nvgBeginFrame(vg, CanvasProperties::winWidth, CanvasProperties::winHeight, pxRatio);
}

void draw_end() {
	nvgEndFrame(vg);
}


extern "C" {
	void create_window(int w, int h){
		if (!glfwInit()) {
			std::cerr << "Couldn't initialize GLFW...\n";
			return;
		};
		
		// Request OpenGL 3.3 Core Profile
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		
		window = glfwCreateWindow(w, h, "NanoVG Square Example", NULL, NULL);
		if (!window) {
			std::cerr << "Couldn't initialize Window...\n";
			glfwTerminate();
			return;
		}
		
		glfwMakeContextCurrent(window);
		
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cerr << "Failed to initialize GLAD\n";
			return;
		}
		
		// 3. Initialize NanoVG
		// NVG_ANTIALIAS makes edges smooth; NVG_STENCIL_STROKES is required for certain fills
		vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
		if (!vg) {
			std::cerr << "Couldn't initialize NanoVG Context...\n";
			glfwDestroyWindow(window);
			glfwTerminate();
			return;
		}

		GLint stencilBits = 0;
		glGetIntegerv(GLFW_STENCIL_BITS, &stencilBits);
		std::cout << "Stencil: " << stencilBits << "\n";
	    draw_prepare();
		nvgGlobalCompositeOperation(vg, NVG_SOURCE_OVER);
	}
	void update_window(){
	    draw_end();
	    glfwPollEvents();
	    glfwSwapBuffers(window);
	    draw_prepare();
	}


	bool should_window_close() {
		return glfwWindowShouldClose(window);
	}
	
	void destroy_window() {
		// auto* ptr = static_cast<Window*>(windowPtr);
		window_destroyed = true;
		nvgDeleteGL3(vg);
		glfwDestroyWindow(window);
		glfwTerminate();
		// delete ptr;
	}
	
	
	void canvas_set_fill_style(const char* c) {
		unsigned int rgb = std::strtoul(c+1, nullptr, 16);
		std::cout << c << " " << rgb << "\n";
		CanvasProperties::fillStyle = nvgRGB(
		    (rgb >> 16) & 0xFF, // 255
		    (rgb >> 8)  & 0xFF, // 0
		    rgb & 0xFF          // 255
		);
	}
	void canvas_set_stroke_style(const char* c) {
		unsigned int rgb = std::strtoul(c+1, nullptr, 16);
		CanvasProperties::strokeStyle = nvgRGB((rgb >> 16) & 0xFF,
			(rgb >> 8)  & 0xFF,
			rgb         & 0xFF);
	}
	
	void canvas_fill_rect(int x, int y, int w, int h){
		if (window_destroyed) return;
		// draw_prepare();
		nvgBeginPath(vg);
		nvgRect(vg,x,y,w,h);
		nvgFillColor(vg, CanvasProperties::fillStyle);
		nvgFill(vg);
		// draw_end();
	}

	void canvas_clear_rect(int x, int y, int w, int h){
		if (window_destroyed) return;
		// draw_prepare();
		nvgBeginPath(vg);
		nvgRect(vg,x,y,w,h);
		nvgFillColor(vg, nvgRGB(0,0,0));
		nvgFill(vg);
		// draw_end();
	}
}