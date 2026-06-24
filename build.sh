#!/usr/bin/sh

clang++ -shared -fPIC ./CPPCanvas/API.cpp \
./CPPCanvas/Thirdparty/skia_build/skia/out/Release/libskia.a \
-I./CPPCanvas/Thirdparty/skia_build/skia/ \
-I./CPPCanvas/Thirdparty/glad/include \
-lGL -lEGL -lglfw -lfontconfig -lfreetype -ldl -lpthread -lm -ljpeg -o ./CPPCanvas/build/BunCanvas.linux.x64.so

