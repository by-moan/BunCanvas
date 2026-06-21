<p align="center">
  <img src="./Assets/logo.png" width="350">
</p>

# BunCanvas

**BunCanvas** is a project focused on bringing the full **HTML Canvas API** to non-browser environments, with a strong emphasis on **1:1 compatibility** with the browser implementation. Designed for the [Bun runtime](https://bun.sh), BunCanvas enables developers to create, render, and manipulate 2D graphics using the same familiar Canvas interfaces—such as `CanvasRenderingContext2D`, paths, transforms, compositing, text rendering, and pixel operations—without relying on a web browser. Its goal is to provide predictable, standards-aligned behavior so existing browser-based canvas code can run with minimal or no modifications, making BunCanvas suitable for native desktop applications, image generation pipelines, and graphics-heavy tooling.

The project relies on [Google Skia](https://github.com/google/skia), being the graphics engine used on google chrome for its use on canvas

This proyect is still in its early infancy, and it core methods from the current canvas implementation are still missing.

## How to build and run (Linux only)
Install depot_tools on your system to build google skia, make sure to add /opt/depot_tools to your PATH. Depending on your system and desktop environment, you will need to adjust the build arguments
``` sh
$ sudo mkdir -p /opt/depot_tools
$ sudo chown -R $USER:$USER /opt/depot_tools
$ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git /opt/depot_tools
```

The following packages are needed in your system to build skia. Install them using your package manager
```
gn ninja
```

Inside CPPCanvas/Thirdparty run the following:
``` sh
$ mkdir -p skia_build && cd skia_build
$ fetch skia
$ cd skia
$ gn gen out/Release --args='is_official_build=true skia_use_system_expat=false skia_use_system_icu=false skia_use_gl=true skia_enable_ganesh=true skia_use_egl=true'
```
After doing the previous steps, the environment is ready for development. Run `build.sh` to generate the `.so` library and test by running `bun ./main.js`