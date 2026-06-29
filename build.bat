@echo off
setlocal enabledelayedexpansion

set PLATFORM=windows
set ARCH=

REM Detect architecture
if /I "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    set ARCH=arm64
) else (
    if /I "%PROCESSOR_ARCHITEW6432%"=="ARM64" (
        set ARCH=arm64
    ) else (
        set ARCH=x64
    )
)

if "%ARCH%"=="x64" (
    set SKIA_LIB=.\CPPCanvas\Thirdparty\skia_build\skia\out\windows_%PROCESSOR_ARCHITECTURE%\skia.lib
    set OUTFILE=.\CPPCanvas\build\BunCanvas.win32.x64.dll
) else (
    set SKIA_LIB=.\CPPCanvas\Thirdparty\skia_build\skia\out\windows_%PROCESSOR_ARCHITEW6432%\skia.lib
    set OUTFILE=.\CPPCanvas\build\BunCanvas.win32.arm64.dll
)

if not exist "%SKIA_LIB%" (
    echo Missing Skia library:
    echo %SKIA_LIB%
    exit /b 1
)

clang-cl ^
 /LD ^
 /std:c++20 ^
 /EHsc ^
 .\CPPCanvas\API.cpp ^
 /I.\CPPCanvas\Thirdparty\skia_build\skia ^
 /I.\CPPCanvas\Thirdparty\glfw-3.4.bin.WIN64\include ^
 "%SKIA_LIB%" ^
 opengl32.lib ^
 user32.lib ^
 gdi32.lib ^
 shell32.lib ^
 .\CPPCanvas\Thirdparty\glfw-3.4.bin.WIN64\lib-vc2022\glfw3_mt.lib ^
 /Fe:%OUTFILE%

if errorlevel 1 (
    echo Build failed
    exit /b 1
)

echo Build succeeded: %OUTFILE%