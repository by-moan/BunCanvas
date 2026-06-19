#!/usr/bin/sh

#clang++ -shared -fPIC ./native.cpp -o build/lib.so
clang++ -shared -fPIC ./CPPCanvas/nanovg.cpp \
./CPPCanvas/Thirdparty/nanovg-master/src/nanovg.c \
./CPPCanvas/Thirdparty/glad/src/glad.c \
-I./CPPCanvas/Thirdparty/nanovg-master/src \
-I./CPPCanvas/Thirdparty/glad/include \
-lGLEW -lGL -lglfw -o ./build/BunCanvas_Linux_x64.so