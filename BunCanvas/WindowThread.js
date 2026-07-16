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

const MessagePost = new JSCallback((instruction)=>{
    self.postMessage(instruction);
    },
    {
        args: ["int"],
        returns: "void"
    }
);
let w;
let h;

self.onmessage = (msg)=>{
    w = msg.data.w
    h = msg.data.h

    const lib = dlopen(path, {
        setup_render_thread: {
            args: ["int","int","ptr"],
            returns: "void",
        }
    });
    lib.symbols.setup_render_thread(w,h,MessagePost.ptr);
    self.postMessage(2)
    process.exit(0)
}

