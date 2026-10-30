import { dlopen, FFIType, JSCallback } from "bun:ffi";

const MessagePost = new JSCallback((instruction)=>{
    self.postMessage(instruction);
    },
    {
        args: ["int"],
        returns: "void"
    }
);

self.onmessage = (msg)=>{
    const { w = 1, h = 1, t = "App", vsync = true, nativePath = "" } = msg.data ?? {};
    
    const lib = dlopen(nativePath, {
        setup_render_thread: {
            args: ["int","int","cstring","ptr","bool"],
            returns: "void",
        }
    });
    lib.symbols.setup_render_thread(w,h,t,MessagePost.ptr,vsync);
    self.postMessage(2)
}

