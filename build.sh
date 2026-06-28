#!/bin/sh

if [[ "$(uname -o)" = "GNU/Linux"  ]]
then
clang++ -shared -fPIC ./CPPCanvas/API.cpp \
./CPPCanvas/Thirdparty/skia_build/skia/out/Release/libskia.a \
-I./CPPCanvas/Thirdparty/skia_build/skia/ \
-I./CPPCanvas/Thirdparty/glad/include \
-lGL -lEGL -lglfw -lfontconfig -lfreetype -ldl -lpthread -lm -ljpeg -o ./CPPCanvas/build/BunCanvas.linux.x64.so
elif [[ "$(uname -o)" = "Darwin"  ]]
then
clang++ -dynamiclib -shared -fPIC ./CPPCanvas/API.cpp \
./CPPCanvas/Thirdparty/skia_build/skia/out/OpenGL_Release/libskia.a \
-std=c++17 \
-stdlib=libc++ \
-DGL_SILENCE_DEPRECATION \
-I./CPPCanvas/Thirdparty/skia_build/skia/ \
-I./CPPCanvas/Thirdparty/glad/include \
-I/usr/local/include \
-L/usr/local/lib \
-framework OpenGL \
-framework CoreFoundation \
-framework CoreGraphics \
-framework CoreText \
-framework CoreServices \
-lglfw -lfreetype -ljpeg -lpthread \
-o ./CPPCanvas/build/BunCanvas.darwin.x64.dylib
fi