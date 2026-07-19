import { lib,encoder, dlPath } from "./symbols.js";

import { ptr, Pointer } from "bun:ffi"
import { Image } from "./Image.js";
import { ImageData } from "./ImageData.js";

const ptrs = new WeakMap();
const canvasFinalizationRegistry = new FinalizationRegistry((ptr: any)=>{
    lib.symbols.canvas_destroy(ptr);
});
const cGradientFinalizationRegistry = new FinalizationRegistry((ptr: any)=>{
    lib.symbols.canvas_gradient_destroy(ptr);
});

class CanvasGradient {
	constructor(x1 : number, y1 : number, x2 : number, y2 : number);
	constructor(...args : any[]){
		let ptr = lib.symbols.canvas_create_linear_gradient(args[0],args[1],args[2],args[3])
		ptrs.set(this,ptr)
		cGradientFinalizationRegistry.register(this,ptr)
	}

	addColorStop(offset : number, color : string) {
		if (offset < 0 || offset > 1) throw new RangeError(`Failed to execute 'addColorStop' on 'CanvasGradient': The provided value (${offset}) is outside the range (0.0, 1.0).`)
		let ptr = lib.symbols.canvas_gradient_add_color_stop(ptrs.get(this),offset,encoder.encode(`${color}\0`))
	}
};

class CanvasRenderingContext2D {
	#lineWidth = 0;
	#globalAlpha = 1;
	#fillStyle = "#000000";
	#strokeStyle = "#000000";
	#compositeOperation = "source-over";
	#fontCss = "10px sans-serif";

	
	
	set font(css: string) {
		this.#fontCss = css.trim().toLowerCase();
		// The native side will parse the CSS string, resolve a typeface and cache it.
		lib.symbols.canvas_set_font(ptrs.get(this), encoder.encode(`${this.#fontCss}\0`));
	}
	get font(): string { return this.#fontCss; }
	
	set fillStyle(style : string | CanvasGradient) {
		if (style.constructor.name == String.name ) lib.symbols.canvas_set_fill_style(ptrs.get(this),encoder.encode(`${style.trim()}\0`))
		if (style instanceof CanvasGradient )lib.symbols.canvas_set_fill_style_gradient(ptrs.get(this),ptrs.get(style))
	}
	set strokeStyle(style : string) {
		lib.symbols.canvas_set_stroke_style(ptrs.get(this),encoder.encode(`${style.trim()}\0`))
	}
	set lineWidth(width : number) {
		lib.symbols.canvas_set_stroke_width(ptrs.get(this),width)
		this.#lineWidth = width
	}
	get lineWidth() : number {
		return this.#lineWidth
	}
	set globalCompositeOperation(operation) {
		if(lib.symbols.canvas_set_composite_operation(ptrs.get(this),encoder.encode(`${operation}\0`)) == true)
			this.#compositeOperation = operation
	}
	
	set globalAlpha(a : number) {
		this.#globalAlpha = lib.symbols.canvas_set_global_alpha(ptrs.get(this),a)
	}
	
	get globalCompositeOperation(){
		return this.#compositeOperation
	}
	
	fillRect(x : number,y : number,w : number,h : number){
		lib.symbols.canvas_fill_rect(ptrs.get(this),x,y,w,h)
	}
	strokeRect(x : number,y : number,w : number,h : number){
		lib.symbols.canvas_stroke_rect(ptrs.get(this),x,y,w,h)
	}
	clearRect(x : number, y : number, w : number, h : number){
		lib.symbols.canvas_clear_rect(ptrs.get(this),x,y,w,h)
	}
	fillText(text:string,x:number,y:number, maxWidth = -1){
		lib.symbols.canvas_fill_text(ptrs.get(this),encoder.encode(`${text}\0`),x,y,maxWidth)
	}
	strokeText(text:string,x:number,y:number, maxWidth = -1){
		lib.symbols.canvas_stroke_text(ptrs.get(this),encoder.encode(`${text}\0`),x,y,maxWidth)
	}
	beginPath(){
		lib.symbols.canvas_path_begin(ptrs.get(this))
	}
	moveTo(x : number,y : number){
		lib.symbols.canvas_path_move_to(ptrs.get(this),x,y)
	}
	lineTo(x : number,y : number){
		lib.symbols.canvas_path_line_to(ptrs.get(this),x,y)
	}
	arc(x : number, y : number, radius : number, startAngle : number, endAngle : number){
		lib.symbols.canvas_path_arc(ptrs.get(this),x, y, radius, startAngle, endAngle)
	}
	arcTo(x1 : number, y1 : number, x2 : number, y2 : number, radius : number){
		lib.symbols.canvas_path_arc_to(ptrs.get(this),x1, y1, x2, y2, radius)
	}
	bezierCurveTo(cp1x : number, cp1y : number, cp2x : number, cp2y : number, x : number, y : number){
		lib.symbols.canvas_path_bezier_to(ptrs.get(this),cp1x, cp1y, cp2x, cp2y, x, y)
	}
	closePath(){
		lib.symbols.canvas_path_close(ptrs.get(this))
	}
	stroke(){
		lib.symbols.canvas_path_stroke(ptrs.get(this))
	}
	set shadowBlur(blur : number){
		lib.symbols.canvas_set_shadow_blur(ptrs.get(this),blur)
	}
	set shadowColor(col : string){
		lib.symbols.canvas_set_shadow_color(ptrs.get(this),encoder.encode(`${col}\0`))
	}
	set shadowOffsetX(x : number){
		lib.symbols.canvas_set_shadow_offsetX(ptrs.get(this),x)
	}
	set shadowOffsetY(y : number){
		lib.symbols.canvas_set_shadow_offsetY(ptrs.get(this),y)
	}
	
	drawImage(img : Image, x : number, y : number, w : number,h : number) : void;
	drawImage(img : Canvas, x : number, y : number, w : number,h : number) : void;
	
	drawImage(img : Image | Canvas, x : number, y : number, w : number,h : number) {
		if (img instanceof Image) {
			lib.symbols.canvas_draw_image_imageType(ptrs.get(this),img.ptr,Number(x),Number(y),Number(w),Number(h))
			return;
		}
		if (img instanceof Canvas) {
			lib.symbols.canvas_draw_image_canvasType(ptrs.get(this),ptrs.get(img),Number(x),Number(y),Number(w),Number(h))
			return;
		}
		
		throw new TypeError("Failed to execute 'drawImage' on 'CanvasRenderingContext2D': The provided value is not of type '(Image or Canvas)'.")
	}
	getImageData(x : number, y : number, w : number,h : number) {
		const arr = new Uint8Array(w*h*4);
		lib.symbols.canvas_get_image_data(ptrs.get(this),x,y,w,h, ptr(arr));
		return new ImageData(w,h,arr);
	}
	putImageData(data : ImageData,dx : number,dy : number ,dirtyX = 0, dirtyY = 0,dirtyWidth = (data instanceof ImageData)?data.width:(()=>{throw new TypeError("Failed to execute 'putImageData' on 'CanvasRenderingContext2D': parameter 1 is not of type 'ImageData'")})(), dirtyHeight = data.height) {
		lib.symbols.canvas_put_image_data(ptrs.get(this),dx,dy,dirtyWidth,dirtyHeight, ptr(data.data));
	}
	save(){
		lib.symbols.canvas_save(ptrs.get(this))
	}
	restore(){
		lib.symbols.canvas_restore(ptrs.get(this))
	}
	translate(x : number,y : number){
		lib.symbols.canvas_translate(ptrs.get(this),x,y)
	}
	rotate(deg : number){
		lib.symbols.canvas_rotate(ptrs.get(this),deg)
	}
	createLinearGradient(x1 : number, y1 : number, x2 : number, y2 : number) : CanvasGradient;
	createLinearGradient(...args : [number,number,number,number]) : CanvasGradient {
		if (args.length < 4) throw new TypeError(`Failed to execute 'createLinearGradient' on 'CanvasRenderingContext2D': 4 arguments required, but only ${args.length} present.`)
		return new CanvasGradient(...args);
	}
	// clearRect(x,y,w,h){
	//     lib.symbols.canvas_clear_rect(x,y,w,h)
	// }
	constructor(iptr : Pointer | null) {
		const renderContextPtr = lib.symbols.canvas_setup_context(iptr, encoder.encode(`2d\0`))
		ptrs.set(this, renderContextPtr)
		lib.symbols.canvas_set_font(ptrs.get(this), encoder.encode(`${this.#fontCss}\0`))
	}
}

{
	const proto = CanvasRenderingContext2D.prototype;
	
	for (const key of Object.getOwnPropertyNames(proto)) {
		const desc = Object.getOwnPropertyDescriptor(proto, key);
		if (key == "constructor"){
			desc?desc.value.toString = () => {return`function ${proto.constructor.name}() { [native code] }`;}:undefined;
			continue
		}
		if (typeof desc?.value === "function") {
			desc.value.toString = () =>
				`function ${key}() { [native code] }`;
		}
	}
}

export class Canvas {
	#dim = new Float64Array(2);
	constructor(w=1,h=1) {
		this.#dim[0] = w;
		this.#dim[1] = h;
		let ptr = lib.symbols.canvas_create(w,h);
		ptrs.set(this,ptr)
		canvasFinalizationRegistry.register(this,ptr)
	}

	get _ptr(){
		return ptrs.get(this)
	}
	
	set width(v){
		this.#dim[0] = v
		lib.symbols.canvas_resize(ptrs.get(this),this.#dim[0],this.#dim[1])
	}
	set height(v){
		this.#dim[1] = v
		lib.symbols.canvas_resize(ptrs.get(this),this.#dim[0],this.#dim[1])
	}
	get width(){
		return this.#dim[0]
	}
	get height(){
		return this.#dim[1]
	}
	
	getContext(contextType : string) {
		if (contextType == "2d") {
			return new CanvasRenderingContext2D(ptrs.get(this))
		}
	}
}

{
	const proto = Canvas.prototype;
	
	for (const key of Object.getOwnPropertyNames(proto)) {
		const desc = Object.getOwnPropertyDescriptor(proto, key);
		if (key == "constructor"){
			desc?desc.value.toString = () => {return`function ${proto.constructor.name}() { [native code] }`;}:undefined;
			continue
		}
		if (typeof desc?.value === "function") {
			desc.value.toString = () =>
				`function ${key}() { [native code] }`;
		}
	}
}




