import { dlopen, FFIType } from "bun:ffi";

export const encoder = new TextEncoder();
export default dlopen("./build/BunCanvas_Linux_x64.so", {
	create_window: {
		args: ["int","int"],
		returns: "void",
	},
	update_window: {
		args: [],
		returns: "void",
	},
	should_window_close: {
		args: [],
		returns: "bool",
	},
	destroy_window: {
		args: [],
		returns: "void",
	},
    canvas_set_fill_style: {
		args: ["cstring"],
		returns: "void",
    },
    canvas_set_stroke_style: {
		args: ["cstring"],
		returns: "void",
    },
    canvas_fill_rect: {
		args: ["int","int","int","int",],
		returns: "void",
    },
    canvas_clear_rect: {
		args: ["int","int","int","int",],
		returns: "void",
    },
});

// const cnv = document.createElement("canvas")

// let ctx= cnv.getContext("2d")

// ctx.fillRect