# BunCanvas Makefile
# Supports Linux (x64/arm64), macOS (x64/arm64), Windows (x64/arm64)

# Platform detection
ifeq ($(OS),Windows_NT)
    PLATFORM := win32
    EXT := dll
    ifeq ($(PROCESSOR_ARCHITEW6432),ARM64)
        ARCH := arm64
    else ifeq ($(PROCESSOR_ARCHITECTURE),ARM64)
        ARCH := arm64
    else
        ARCH := x64
    endif
    IS_WINDOWS := 1
else
    UNAME_O := $(shell uname -o)
    UNAME_M := $(shell uname -m)
    
    ifeq ($(UNAME_O),GNU/Linux)
        PLATFORM := linux
        EXT := so
    else ifeq ($(UNAME_O),Darwin)
        PLATFORM := darwin
        EXT := dylib
    else
        $(error Unsupported platform: $(UNAME_O))
    endif
    
    ifeq ($(UNAME_M),x86_64)
        ARCH := x64
    else
        ARCH := arm64
    endif
    
    IS_WINDOWS := 0
endif

# Directories
ROOT := .
CANVAS_DIR := $(ROOT)/CPPCanvas
BUILD_DIR := $(CANVAS_DIR)/build
OUTPUT_DIR := $(ROOT)/build
SRC_DIR := $(CANVAS_DIR)/src

# Source files for dependency tracking
API_CPP := $(CANVAS_DIR)/API.cpp
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
ALL_SOURCES := $(API_CPP) $(SRC_FILES)

# Target library paths
TARGET := $(BUILD_DIR)/BunCanvas.$(PLATFORM).$(ARCH).$(EXT)
COPIED_LIB := $(OUTPUT_DIR)/BunCanvas.$(PLATFORM).$(ARCH).$(EXT)

.PHONY: all build run run_dry clean

all: build

# Build only
build: $(TARGET)

# Build (if needed) and run
run: $(COPIED_LIB)
	echo $(OUTPUT_DIR)
	cd $(OUTPUT_DIR) && bun ../main.js
# 	cd $(OUTPUT_DIR) && gdb -ex "set pagination off" -ex "handle SIGPWR nostop noprint pass" --args bun ../main.js bun ../main.js

# Run without building
run_dry:
	@mkdir -p $(OUTPUT_DIR)
	cp $(TARGET) $(COPIED_LIB)
	cd $(OUTPUT_DIR) && bun ../main.js

# Copy to build directory (triggered by run when TARGET is newer)
$(COPIED_LIB): $(TARGET)
	@mkdir -p $(OUTPUT_DIR)
	cp $(TARGET) $(COPIED_LIB)

clean:
	rm -f $(TARGET) $(COPIED_LIB)

# Windows build (clang-cl)
ifeq ($(IS_WINDOWS),1)
    ifeq ($(ARCH),x64)
        SKIA_LIB := $(CANVAS_DIR)/Thirdparty/skia_build/skia/out/windows_AMD64/skia.lib
    else
        SKIA_LIB := $(CANVAS_DIR)/Thirdparty/skia_build/skia/out/windows_arm64/skia.lib
    endif
    
    $(TARGET): $(ALL_SOURCES)
		@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
		clang-cl /LD /std:c++20 /EHsc $(API_CPP) \
			/I$(CANVAS_DIR)/Thirdparty/skia_build/skia \
			/I$(CANVAS_DIR)/Thirdparty/glfw-3.4.bin.WIN64\include \
			/I$(CANVAS_DIR)/Thirdparty/miniaudio \
			"$(SKIA_LIB)" \
			opengl32.lib user32.lib gdi32.lib shell32.lib dwrite.lib \
			$(CANVAS_DIR)/Thirdparty\glfw-3.4.bin.WIN64\lib-vc2022\glfw3_mt.lib \
			/Fe:$@

# Linux build
else ifeq ($(PLATFORM),linux)
    SKIA_OUT := $(CANVAS_DIR)/Thirdparty/skia_build/skia/out/$(UNAME_O)_$(UNAME_M)
    GLFW_LIB := $(CANVAS_DIR)/Thirdparty/glfw/build/src/libglfw3.a
    
    ifeq ($(ARCH),x64)
        $(TARGET): $(ALL_SOURCES)
			@mkdir -p $(BUILD_DIR)
			clang++ -shared -fPIC $(API_CPP) \
				"$(SKIA_OUT)/libskia.a" \
				"$(GLFW_LIB)" \
				-std=c++23 -O0 \
				-I$(CANVAS_DIR)/Thirdparty/skia_build/skia/ \
				-I$(CANVAS_DIR)/Thirdparty/glad/include \
				-L/usr/lib/clang/22/lib/linux \
				-lGL -lEGL -lfontconfig -lfreetype -ldl -lpthread -lm -ljpeg \
				-o $@
    else
        $(TARGET): $(ALL_SOURCES)
			@mkdir -p $(BUILD_DIR)
			clang++ -shared -fPIC $(API_CPP) \
				"$(SKIA_OUT)/libskia.a" \
				"$(GLFW_LIB)" \
				-std=c++23 -O3 \
				-I$(CANVAS_DIR)/Thirdparty/skia_build/skia/ \
				-I$(CANVAS_DIR)/Thirdparty/glad/include \
				-lGL -lEGL -lglfw -lGLEW -lfontconfig -lfreetype -ldl -lpthread -lm -ljpeg \
				-o $@
    endif

# macOS build
else ifeq ($(PLATFORM),darwin)
    SKIA_OUT := $(CANVAS_DIR)/Thirdparty/skia_build/skia/out/$(UNAME_O)_$(UNAME_M)
    
    ifeq ($(ARCH),x64)
        $(TARGET): $(ALL_SOURCES)
			@mkdir -p $(BUILD_DIR)
			clang++ -dynamiclib -shared -fPIC $(API_CPP) \
				"$(SKIA_OUT)/libskia.a" \
				-std=c++23 -g -O3 -stdlib=libc++ \
				-DGL_SILENCE_DEPRECATION \
				-I$(CANVAS_DIR)/Thirdparty/skia_build/skia/ \
				-I$(CANVAS_DIR)/Thirdparty/glad/include \
				-I/usr/local/include \
				-L/usr/local/lib \
				-framework OpenGL -framework CoreFoundation -framework CoreGraphics \
				-framework CoreText -framework CoreServices \
				-lglfw -lfreetype -ljpeg -lpthread \
				-o $@
    else
        $(TARGET): $(ALL_SOURCES)
			@mkdir -p $(BUILD_DIR)
			clang++ -dynamiclib -shared -fPIC $(API_CPP) \
				"$(SKIA_OUT)/libskia.a" \
				-std=c++23 -stdlib=libc++ \
				-DGL_SILENCE_DEPRECATION \
				-I$(CANVAS_DIR)/Thirdparty/skia_build/skia/ \
				-I$(CANVAS_DIR)/Thirdparty/glad/include \
				-I/opt/homebrew/include \
				-L/opt/homebrew/lib \
				-framework OpenGL -framework CoreFoundation -framework CoreGraphics \
				-framework CoreText -framework CoreServices \
				-lglfw -lfreetype -ljpeg -lpthread \
				-o $@
    endif
endif