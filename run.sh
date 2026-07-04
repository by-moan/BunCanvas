#!/bin/sh

PLATFORM=""
EXT=""
ARCH=""

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

if [[ "$(uname -m)" = "x86_64" ]]
then
ARCH="x64"
else
ARCH="arm64"
fi

cp ./CPPCanvas/build/BunCanvas.$PLATFORM.$ARCH.$EXT ./build/
cd ./build/

# bun ../main.js

gdb -q -batch -ex "set pagination off" -ex "run" --args bun ../main.js