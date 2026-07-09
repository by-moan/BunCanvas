#!/bin/sh

PLATFORM=""
EXT=""

if [[ "$(uname -o)" = "GNU/Linux"  ]]
then
PLATFORM="linux"
EXT="so"

elif [[ "$(uname -o)" = "Darwin"  ]]
then
PLATFORM="darwin"
EXT="dylib"

else
echo "BunCanvas is not compatible with this platform..."
exit
fi

if [[ "$(uname -o)" = "GNU/Linux"  ]]
then
    if [[ "$(uname -m)" = "x86_64" ]]
    then
    clang++ -shared -fPIC ./CPPCanvas/API.cpp \
    "./CPPCanvas/Thirdparty/skia_build/skia/out/$(uname -o)_$(uname -m)/libskia.a" \
    -std=c++23 \
    -I./CPPCanvas/Thirdparty/skia_build/skia/ \
    -I./CPPCanvas/Thirdparty/glad/include \
    -lGL -lEGL -lglfw -lGLEW -lfontconfig -lfreetype -ldl -lpthread -lm -ljpeg -o "./CPPCanvas/build/BunCanvas.$PLATFORM.x64.so"
    elif [[ "$(uname -m)" = "arm64" ]]
    then
    clang++ -shared -fPIC ./CPPCanvas/API.cpp \
    "./CPPCanvas/Thirdparty/skia_build/skia/out/$(uname -o)_$(uname -m)/libskia.a" \
    -std=c++23 \
    -I./CPPCanvas/Thirdparty/skia_build/skia/ \
    -I./CPPCanvas/Thirdparty/glad/include \
    -lGL -lEGL -lglfw -lGLEW -lfontconfig -lfreetype -ldl -lpthread -lm -ljpeg -o "./CPPCanvas/build/BunCanvas.$PLATFORM.arm64.so"
    fi
elif [[ "$(uname -o)" = "Darwin" ]]
then
    if [[ "$(uname -m)" = "x86_64" ]]
    then
    clang++ -dynamiclib -shared -fPIC ./CPPCanvas/API.cpp \
    "./CPPCanvas/Thirdparty/skia_build/skia/out/$(uname -o)_$(uname -m)/libskia.a" \
    -std=c++23 \
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
    -o "./CPPCanvas/build/BunCanvas.$PLATFORM.x64.dylib"
    else
    clang++ -dynamiclib -shared -fPIC ./CPPCanvas/API.cpp \
    "./CPPCanvas/Thirdparty/skia_build/skia/out/$(uname -o)_$(uname -m)/libskia.a" \
    -std=c++23 \
    -stdlib=libc++ \
    -DGL_SILENCE_DEPRECATION \
    -I./CPPCanvas/Thirdparty/skia_build/skia/ \
    -I./CPPCanvas/Thirdparty/glad/include \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    -framework OpenGL \
    -framework CoreFoundation \
    -framework CoreGraphics \
    -framework CoreText \
    -framework CoreServices \
    -lglfw -lfreetype -ljpeg -lpthread \
    -o "./CPPCanvas/build/BunCanvas.$PLATFORM.arm64.dylib"
    fi
fi