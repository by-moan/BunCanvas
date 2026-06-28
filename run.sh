#!/bin/sh

if [[ "$(uname -o)" = "GNU/Linux"  ]]
then
cp ./CPPCanvas/build/BunCanvas.linux.x64.so ./build/Linux

cd ./build/Linux
bun ../../main.js
elif [[ "$(uname -o)" = "Darwin"  ]]
then
cp ./CPPCanvas/build/BunCanvas.darwin.x64.dylib ./build/MacOS

cd ./build/MacOS
bun ../../main.js
else
echo "BunCanvas is not compatible with this platform..."
fi
