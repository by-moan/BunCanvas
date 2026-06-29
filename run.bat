@echo off
setlocal

set PLATFORM=win32
set EXT=dll
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

set SOURCE=.\CPPCanvas\build\BunCanvas.%PLATFORM%.%ARCH%.%EXT%
set DEST=.\build\

if not exist "%SOURCE%" (
    echo Missing file:
    echo %SOURCE%
    exit /b 1
)

copy /Y "%SOURCE%" "%DEST%"
if errorlevel 1 exit /b 1

cd /d .\build
bun ..\main.js