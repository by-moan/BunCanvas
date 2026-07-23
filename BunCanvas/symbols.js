import { dlopen, FFIType } from "bun:ffi";

let extension;

if (process.platform === "win32") {
	extension = "dll"
}else if (process.platform === "darwin") {
	extension = "dylib"
}else if (process.platform === "linux") {
	extension = "so"
}

export const dlPath = `BunCanvas.${process.platform}.${process.arch}.${extension}`;

const exists = await Bun.file(dlPath).exists();

if(!exists) {
	console.error(`${dlPath} was not found in the working directory!`)
	process.exit(-1)
}

export const encoder = new TextEncoder();

export const lib = dlopen(dlPath, {
	get_wResizeViewer:{
		args: [],
		returns: "ptr",
	},
	get_mMoveViewer:{
		args: [],
		returns: "ptr",
	},
	get_mClickViewer:{
		args: [],
		returns: "ptr",
	},
	get_mDownViewer:{
		args: [],
		returns: "ptr",
	},
	get_mUpViewer:{
		args: [],
		returns: "ptr",
	},
	get_kDownViewer:{
		args: [],
		returns: "ptr",
	},
	get_kUpViewer:{
		args: [],
		returns: "ptr",
	},
	signal_frame_ready:{
		args: [],
		returns: "void",
	},
	set_vsync: {
		args: ["bool"],
		returns: "void",
	},
	create_window: {
		args: ["int","int", "cstring"],
		returns: "void",
	},
	update_window: {
		args: ["ptr"],
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
	canvas_init_gpu_context: {
		args: [],
		returns: "bool",
	},
	canvas_flush_gpu: {
		args: [],
		returns: "void",
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
		args: ["ptr", "cstring", "ptr"],
		returns: "ptr",
	},
	canvas_set_fill_style: {
		args: ["ptr","cstring"],
		returns: "bool",
	},
	canvas_set_fill_style_gradient: {
		args: ["ptr","ptr"],
		returns: "bool",
	},
	canvas_set_stroke_style: {
		args: ["ptr","cstring"],
		returns: "bool",
	},
	canvas_set_stroke_style_gradient: {
		args: ["ptr","ptr"],
		returns: "bool",
	},
	canvas_set_stroke_width: {
		args: ["ptr","float"],
		returns: "bool",
	},
	canvas_fill_rect: {
		args: ["ptr","int","int","int","int"],
		returns: "void",
	},
	canvas_stroke_rect: {
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
	canvas_draw_image_imageType: {
		args: ["ptr","ptr","float","float","float","float"],
		returns: "void",
	},
	canvas_draw_image_canvasType: {
		args: ["ptr","ptr","float","float","float","float"],
		returns: "void",
	},
	canvas_get_image_data: {
		args: ["ptr","int","int","int","int", "ptr"],
		returns: "bool",
	},
	// canvas_put_image_data(void* canvasObj, int x, int y, int w, int h, uint8_t* buffer)
	canvas_put_image_data: {
		args: ["ptr","int","int","int","int", "ptr"],
		returns: "bool",
	},
	canvas_set_global_alpha: {
		args: ["ptr","float"],
		returns: "float",
	},
	canvas_save: {
		args: ["ptr"],
		returns: "float",
	},
	canvas_restore: {
		args: ["ptr"],
		returns: "float",
	},
	canvas_translate: {
		args: ["ptr","float","float"],
		returns: "float",
	},
	canvas_rotate: {
		args: ["ptr","float"],
		returns: "float",
	},
	canvas_set_font: {
		args: ["ptr","cstring"],
		returns: "bool",
	},
	canvas_fill_text: {
		args: ["ptr","cstring","float","float","float"],
		returns: "void",
	},
	canvas_stroke_text: {
		args: ["ptr","cstring","float","float","float"],
		returns: "void",
	},
	canvas_set_shadow_blur: {
		args: ["ptr","float"],
		returns: "void",
	},
	canvas_set_shadow_color: {
		args: ["ptr","cstring"],
		returns: "void",
	},
	canvas_set_shadow_offsetX: {
		args: ["ptr","float"],
		returns: "void",
	},
	canvas_set_shadow_offsetY: {
		args: ["ptr","float"],
		returns: "void",
	},
	canvas_set_image_smoothing_quality: {
		args: ["ptr","int"],
		returns: "bool",
	},
	canvas_set_image_smoothing_enabled: {
		args: ["ptr","bool"],
		returns: "bool",
	},
	canvas_create_linear_gradient: {
		args: ["float","float","float","float"],
		returns: "ptr",
	},
	canvas_gradient_add_color_stop: {
		args: ["ptr", "float", "cstring"],
		returns: "bool",
	},
	canvas_gradient_destroy: {
		args: ["ptr"],
		returns: "void",
	},
	canvas_get_transform: {
		args: ["ptr", "ptr"],
		returns: "bool",
	},
	canvas_set_filter: {
		args: ["ptr", "cstring"],
		returns: "bool",
	},
	canvas_destroy: {
		args: ["ptr"],
		returns: "void",
	},
	image_create: {
		args: [],
		returns: "ptr",
	},
	image_set_src: {
		args: ["ptr","cstring"],
		returns: "bool",
	},
	image_destroy: {
		args: ["ptr"],
		returns: "void",
	},
	font_load:{
		args: ["cstring"],
		returns: "ptr",
	},
	// font_create_from_face: {
	// 	args: ["cstring","cstring","int","int","int","cstring","cstring","cstring",],
	// 	returns: "ptr",
	// },
	font_add_to_registry: {
		args: ["cstring","ptr"],
		returns: "void",
	},
});

// const cnv = document.createElement("canvas")

// let ctx= cnv.getContext("2d")

// ctx.fillRect