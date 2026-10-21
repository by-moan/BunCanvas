import { dlopen, FFIType, suffix } from "bun:ffi";
import { getNativePath } from "./loader";

const candidates = [
	`./node_modules/BunCanvas/bin/BunCanvas.${process.arch}.${suffix}`,
	`./BunCanvas/bin/BunCanvas.${process.arch}.${suffix}`,
	`BunCanvas.${process.arch}.${suffix}`,
];

let selected;

for (const item of candidates) {
	let exists = await Bun.file(item).exists()
	if (exists) {
		selected = item
		break
	}
}

if(!selected) {
	console.error(`BunCanvas.${process.arch}.${suffix} binary was not found in the working directory!`)
	process.exit(-1)
}

export const dlPath = selected

export const encoder = new TextEncoder();

const encodedCache = new Map();

export function encodeCString(str) {
    let buf = encodedCache.get(str);
    if (buf) return buf;

    buf = encoder.encode(str + "\0");
    encodedCache.set(str, buf);
    return buf;
}

export const lib = dlopen(dlPath, {
	glfw_init: {
        args: [],
        returns: "void"
    },
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
	get_mWheelViewer:{
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
	canvas_get_context2D: {
		args: ["ptr", "ptr", "bool", "bool"],
		returns: "ptr",
	},
	canvas_set_fill_style: {
		args: ["ptr",'cstring'],
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
	canvas_path_quad_to: {
		args: ["ptr", "float", "float","float","float"],
		returns: "void",
	},
	canvas_path_ellipse: {
		args: ["ptr", "float", "float","float","float","float","float","float","bool"],
		returns: "void",
	},
	canvas_path_fill: {
		args: ["ptr", "ptr"],
		returns: "void",
	},
	canvas_path_stroke: {
		args: ["ptr", "ptr"],
		returns: "void",
	},
	canvas_path_close: {
		args: ["ptr"],
		returns: "void",
	},
	canvas_path_rect: {
		args: ["ptr", "float", "float", "float", "float"],
		returns: "void",
	},
	canvas_path_roundrect: {
		args: ["ptr", "float", "float", "float", "float", "float"],
		returns: "void",
	},
	canvas_path_roundrect_array: {
		args: ["ptr", "float", "float", "float", "float", "ptr", "int"],
		returns: "void",
	},
	canvas_set_composite_operation: {
		args: ["ptr", "cstring"],
		returns: "bool",
	},
	canvas_draw_image_imageType: {
		args: ["ptr", "ptr", "ptr", "int"],
		returns: "void",
	},
	canvas_draw_image_canvasType: {
		args: ["ptr", "ptr","ptr", "int"],
		returns: "void",
	},
	canvas_get_image_data: {
		args: ["ptr","int","int","int","int", "ptr"],
		returns: "bool",
	},
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
		returns: "void",
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
	canvas_create_conic_gradient: {
		args: ["float","float","float"],
		returns: "ptr",
	},
	canvas_create_linear_gradient: {
		args: ["float","float","float","float"],
		returns: "ptr",
	},
	canvas_create_radial_gradient: {
		args: ["float","float","float","float","float","float"],
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
		returns: "void",
	},
	canvas_set_transform: {
		args: ["ptr", "ptr"],
		returns: "void",
	},
	canvas_reset_transform: {
		args: ["ptr"],
		returns: "void",
	},
	canvas_set_filter: {
		args: ["ptr", "cstring"],
		returns: "bool",
	},
	canvas_set_line_dash: {
		args: ["ptr", "ptr", "int", "float"],
		returns: "bool",
	},
	canvas_set_line_join: {
		args: ["ptr","int"],
		returns: "bool",
	},
	canvas_path_clip: {
		args: ["ptr"],
		returns: "void",
	},
	canvas_set_text_baseline: {
		args: ["ptr", "int"],
		returns: "void",
	},
	canvas_set_text_align: {
		args: ["ptr", "int"],
		returns: "void",
	},
	canvas_measure_text: {
		args: ["ptr", "cstring", "ptr"],
		returns: "void",
	},
	canvas_destroy: {
		args: ["ptr"],
		returns: "void",
	},
	image_create: {
		args: [],
		returns: "ptr",
	},
	images_cleanup: {
		args: [],
		returns: "void",
	},
	image_set_src: {
		args: ["ptr","cstring", "bool"],
		returns: "void",
	},
	image_load: {
		args: ["ptr","ptr"],
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

	audio_backend_init : {
		args: [],
		returns: "void",
	},
	audio_create : {
		args: ["cstring"],
		returns: "ptr",
	},
	audio_set_src : {
		args: ["ptr","cstring"],
		returns: "void",
	},
	audio_play : {
		args: ["ptr"],
		returns: "void",
	},
	audio_stop : {
		args: ["ptr"],
		returns: "void",
	},
	audio_destroy : {
		args: ["ptr"],
		returns: "void",
	},

	path2d_create : {
		args: [],
		returns: "ptr",
	},
	path2d_create_from_copy : {
		args: ["ptr"],
		returns: "ptr",
	},
	path2d_destroy : {
		args: ["ptr"],
		returns: "void",
	},

	path2d_move_to: {
		args: ["ptr", "int", "int"],
		returns: "void",
	},
	path2d_line_to: {
		args: ["ptr", "int", "int"],
		returns: "void",
	},
	path2d_arc: {
		args: ["ptr", "float", "float","float","float","float"],
		returns: "void",
	},
	path2d_arc_to: {
		args: ["ptr", "float", "float","float","float","float"],
		returns: "void",
	},
	path2d_bezier_to: {
		args: ["ptr", "float", "float","float","float","float","float"],
		returns: "void",
	},
	path2d_quad_to: {
		args: ["ptr", "float", "float","float","float"],
		returns: "void",
	},
	path2d_ellipse: {
		args: ["ptr", "float", "float","float","float","float","float","float","bool"],
		returns: "void",
	},
	path2d_close: {
		args: ["ptr"],
		returns: "void",
	},
	path2d_rect: {
		args: ["ptr", "float", "float", "float", "float"],
		returns: "void",
	},
	path2d_roundrect: {
		args: ["ptr", "float", "float", "float", "float", "float"],
		returns: "void",
	},
	path2d_roundrect_array: {
		args: ["ptr", "float", "float", "float", "float", "ptr", "int"],
		returns: "void",
	},
	font_add_to_registry: {
		args: ["cstring","ptr"],
		returns: "void",
	},
});