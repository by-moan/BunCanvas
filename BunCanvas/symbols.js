import { dlopen, FFIType } from "bun:ffi";

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

export const encoder = new TextEncoder();

export const lib = dlopen(path, {
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
	window_query_events: {
		args: [],
		returns: "cstring",
	},
	canvas_create: {
		args: ["int","int"],
		returns: "ptr",
	},
	canvas_resize: {
		args: ["ptr","int","int"],
		returns: "void",
	},
	canvas_append: {
		args: ["ptr",],
		returns: "void",
	},
	canvas_setup_context: {
		args: ["ptr",],
		returns: "void",
	},
	canvas_set_fill_style: {
		args: ["ptr","cstring"],
		returns: "bool",
	},
	canvas_set_stroke_style: {
		args: ["ptr","cstring"],
		returns: "bool",
	},
	canvas_set_stroke_width: {
		args: ["ptr","float"],
		returns: "void",
	},
	canvas_fill_rect: {
		args: ["ptr","int","int","int","int"],
		returns: "void",
	},
	canvas_clear_rect: {
		args: ["ptr","int","int","int","int"],
		returns: "void",
	},
	canvas_path_begin: {
		args: ["ptr"],
		returns: "void",
	},
	canvas_path_move_to: {
		args: ["ptr", "int", "int"],
		returns: "void",
	},
	canvas_path_line_to: {
		args: ["ptr", "int", "int"],
		returns: "void",
	},
	canvas_path_arc: {
		args: ["ptr", "float", "float","float","float","float"],
		returns: "void",
	},
	canvas_path_arc_to: {
		args: ["ptr", "float", "float","float","float","float"],
		returns: "void",
	},
	canvas_path_bezier_to: {
		args: ["ptr", "float", "float","float","float","float","float"],
		returns: "void",
	},
	canvas_path_stroke: {
		args: ["ptr"],
		returns: "void",
	},
	canvas_path_close: {
		args: ["ptr"],
		returns: "void",
	},
	canvas_set_composite_operation: {
		args: ["ptr", "cstring"],
		returns: "bool",
	},
	canvas_draw_image: {
		args: ["ptr","ptr","int","int","int","int"],
		returns: "void",
	},
	image_load: {
		args: ["cstring"],
		returns: "ptr",
	},
	
});

// const cnv = document.createElement("canvas")

// let ctx= cnv.getContext("2d")

// ctx.fillRect