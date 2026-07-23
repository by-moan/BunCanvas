import { dlopen, FFIType, JSCallback } from "bun:ffi";

let extension;

if (process.platform === "win32") {
    extension = "dll"
}else if (process.platform === "darwin") {
    extension = "dylib"
}else if (process.platform === "linux") {
    extension = "so"
}

const path = `BunCanvas.${process.platform}.${process.arch}.${extension}`;

const exists = await Bun.file(path).exists();

if(!exists) {
    console.error(`${path} was not found in the working directory!`)
    process.exit(-1)
}

const lib = dlopen(path, {
    glfw_init: {
        args: [],
        returns: "void"
    },
    setup_render_thread: {
        args: ["int","int","cstring","ptr","bool"],
        returns: "void",
    }
});

// lib.symbols.glfw_init();

const MessagePost = new JSCallback((instruction)=>{
    self.postMessage(`${instruction}`);
    },
    {
        args: ["int"],
        returns: "void"
    }
);
let w;
let h;
let t;
let vsync;

self.onmessage = (msg)=>{
    const { w = 1, h = 1, t = "App", vsync = true } = msg.data ?? {};
    
    lib.symbols.setup_render_thread(w,h,t,MessagePost.ptr,vsync);
    self.postMessage("2")
    process.exit(0)
}

