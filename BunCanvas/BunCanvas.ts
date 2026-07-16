import { lib,encoder, dlPath } from "./symbols.js";
import { Event } from "./Event.js";
import { MouseEvent } from "./MouseEvent.js"

import { dlopen, ptr, toArrayBuffer, JSCallback, FFIType, Pointer } from "bun:ffi"
import { KeyboardEvent } from "./KeyboardEvent";
import { FontFace } from "./FontFace.js";

import WindowThread from "./WindowThread.js" with { type:"text" };

let requestedFrames : any[] = []
const ptrs = new WeakMap();

export class Image {
	constructor() {
		ptrs.set(this,lib.symbols.image_create())
	}
	
	onload: ((this: Image, evt: Event) => any) | null = null;
	onerror: ((this: Image, evt: Event) => any) | null = null;
	
	set src(path : string){
		if (lib.symbols.image_set_src(ptrs.get(this), encoder.encode(`${path}\0`))) {
			this.onload instanceof Function ? this.onload(new Event("load",this)):undefined;
		}else {
			this.onerror instanceof Function ? this.onerror(new Event("error",this)):undefined;
		}
	}
}

export class ImageData {
	width : number;
	height : number;
	data : Uint8Array;
	constructor(width : number,height : number,data : Uint8Array){
		this.width = width;
		this.height = height;
		this.data = data;
		
	}
}

class CanvasRenderingContext2D {
	#iptr : Pointer | null;
	#lineWidth = 0;
	#globalAlpha = 1;
	#fillStyle = "#000000";
	#strokeStyle = "#000000";
	#compositeOperation = "source-over";
	#fontCss = "10px sans-serif";

	
	
	set font(css: string) {
		this.#fontCss = css.trim().toLowerCase();
		// The native side will parse the CSS string, resolve a typeface and cache it.
		lib.symbols.canvas_set_font(this.#iptr, encoder.encode(`${this.#fontCss}\0`));
	}
	get font(): string { return this.#fontCss; }
	
	set fillStyle(style : string) {
		lib.symbols.canvas_set_fill_style(this.#iptr,encoder.encode(`${style.trim()}\0`))
	}
	set strokeStyle(style : string) {
		lib.symbols.canvas_set_stroke_style(this.#iptr,encoder.encode(`${style.trim()}\0`))
	}
	set lineWidth(width : number) {
		lib.symbols.canvas_set_stroke_width(this.#iptr,width)
		this.#lineWidth = width
	}
	get lineWidth() : number {
		return this.#lineWidth
	}
	set globalCompositeOperation(operation) {
		if(lib.symbols.canvas_set_composite_operation(this.#iptr,encoder.encode(`${operation}\0`)) == true)
			this.#compositeOperation = operation
	}
	
	set globalAlpha(a : number) {
		this.#globalAlpha = lib.symbols.canvas_set_global_alpha(this.#iptr,a)
	}
	
	get globalCompositeOperation(){
		return this.#compositeOperation
	}
	
	fillRect(x : number,y : number,w : number,h : number){
		lib.symbols.canvas_fill_rect(this.#iptr,x,y,w,h)
	}
	clearRect(x : number, y : number, w : number, h : number){
		lib.symbols.canvas_clear_rect(this.#iptr,x,y,w,h)
	}
	fillText(text:string,x:number,y:number, maxWidth = -1){
		lib.symbols.canvas_fill_text(this.#iptr,encoder.encode(`${text}\0`),x,y,maxWidth)
	}
	beginPath(){
		lib.symbols.canvas_path_begin(this.#iptr)
	}
	moveTo(x : number,y : number){
		lib.symbols.canvas_path_move_to(this.#iptr,x,y)
	}
	lineTo(x : number,y : number){
		lib.symbols.canvas_path_line_to(this.#iptr,x,y)
	}
	arc(x : number, y : number, radius : number, startAngle : number, endAngle : number){
		lib.symbols.canvas_path_arc(this.#iptr,x, y, radius, startAngle, endAngle)
	}
	arcTo(x1 : number, y1 : number, x2 : number, y2 : number, radius : number){
		lib.symbols.canvas_path_arc_to(this.#iptr,x1, y1, x2, y2, radius)
	}
	bezierCurveTo(cp1x : number, cp1y : number, cp2x : number, cp2y : number, x : number, y : number){
		lib.symbols.canvas_path_bezier_to(this.#iptr,cp1x, cp1y, cp2x, cp2y, x, y)
	}
	closePath(){
		lib.symbols.canvas_path_close(this.#iptr)
	}
	stroke(){
		lib.symbols.canvas_path_stroke(this.#iptr)
	}
	
	drawImage(img : Image, x : number, y : number, w : number,h : number) : void;
	drawImage(img : Canvas, x : number, y : number, w : number,h : number) : void;
	
	drawImage(img : Image | Canvas, x : number, y : number, w : number,h : number) {
		if (img instanceof Image) {
			lib.symbols.canvas_draw_image_imageType(this.#iptr,ptrs.get(img),Number(x),Number(y),Number(w),Number(h))
			return;
		}
		if (img instanceof Canvas) {
			lib.symbols.canvas_draw_image_canvasType(this.#iptr,ptrs.get(img),Number(x),Number(y),Number(w),Number(h))
			return;
		}
		
		throw new TypeError("Failed to execute 'drawImage' on 'CanvasRenderingContext2D': The provided value is not of type '(Image or Canvas)'.")
	}
	getImageData(x : number, y : number, w : number,h : number) {
		const arr = new Uint8Array(w*h*4);
		lib.symbols.canvas_get_image_data(this.#iptr,x,y,w,h, ptr(arr));
		return new ImageData(w,h,arr);
	}
	putImageData(data : ImageData,dx : number,dy : number ,dirtyX = 0, dirtyY = 0,dirtyWidth = (data instanceof ImageData)?data.width:(()=>{throw new TypeError("Failed to execute 'putImageData' on 'CanvasRenderingContext2D': parameter 1 is not of type 'ImageData'")})(), dirtyHeight = data.height) {
		lib.symbols.canvas_put_image_data(this.#iptr,dx,dy,dirtyWidth,dirtyHeight, ptr(data.data));
	}
	save(){
		lib.symbols.canvas_save(this.#iptr)
	}
	restore(){
		lib.symbols.canvas_restore(this.#iptr)
	}
	translate(x : number,y : number){
		lib.symbols.canvas_translate(this.#iptr,x,y)
	}
	rotate(deg : number){
		lib.symbols.canvas_rotate(this.#iptr,deg)
	}
	// clearRect(x,y,w,h){
	//     lib.symbols.canvas_clear_rect(x,y,w,h)
	// }
	constructor(iptr : Pointer | null) {
		
		
		const renderContextPtr = lib.symbols.canvas_setup_context(iptr, encoder.encode(`2d\0`))
		ptrs.set(this, renderContextPtr)
		this.#iptr = renderContextPtr
		lib.symbols.canvas_set_font(this.#iptr, encoder.encode(`${this.#fontCss}\0`))
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
	#iptr;
	#dim = new Float64Array(2);
	constructor(w=1,h=1) {
		this.#iptr = lib.symbols.canvas_create(w,h)
		this.#dim[0] = w;
		this.#dim[1] = h;
		ptrs.set(this,this.#iptr)
	}
	
	set width(v){
		this.#dim[0] = v
		lib.symbols.canvas_resize(this.#iptr,this.#dim[0],this.#dim[1])
	}
	set height(v){
		this.#dim[1] = v
		lib.symbols.canvas_resize(this.#iptr,this.#dim[0],this.#dim[1])
	}
	get width(){
		console.log("setter!")
		return this.#dim[0]
	}
	get height(){
		return this.#dim[1]
	}
	
	getContext(contextType : string) {
		if (contextType == "2d") {
			return new CanvasRenderingContext2D(this.#iptr)
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

export class Window {
	#dim = new Float64Array(2);
	
	#rEvts = new Map();
	#mMoveEvts = new Map();
	#mClickEvts = new Map();
	#keyDownEvts = new Map();
	
	#evtDetails = new WeakMap();
	
	#freeIndex = new Array();
	#eIndex = 0

	fonts = {
		add(font : FontFace){
			if (!font._nativePtr) throw new TypeError("Font was not loaded");
			lib.symbols.font_add_to_registry(encoder.encode(`${font.family}\0`),font._nativePtr)
		}
	}
	
	onresize : ((this: Window, evt: Event) => any) | null = null;
	onmousemove : ((evt: MouseEvent) => any) | null = null;
	onclick : ((evt: MouseEvent) => any) | null = null;
	onkeydown : ((evt: KeyboardEvent) => any) | null = null;
	
	get innerWidth(){
		return this.#dim[0]
	}
	get innerHeight(){
		return this.#dim[1]
	}
	
	#wResizeViewer = new Int32Array(toArrayBuffer(lib.symbols.get_wResizeViewer()!,0, 3 * 4));
	#mMoveViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mMoveViewer()!,0, 9 * 8));
	#mClickViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mClickViewer()!,0, 9 * 8));
	#mDownViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mDownViewer()!,0, 9 * 8));
	#mUpViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mUpViewer()!,0, 9 * 8));
	#kDownViewer = new Int32Array(toArrayBuffer(lib.symbols.get_kDownViewer()!,0, 7 * 4));
	#kUpViewer = new Int32Array(toArrayBuffer(lib.symbols.get_kUpViewer()!,0, 7 * 4));
	
	#renderThread : Worker | null;
	
	constructor(width : number, height : number, title = "App") {
		this.#dim[0] = width;
		this.#dim[1] = height;
		
		if (process.platform !== "darwin") {
			let gpuInitialized = false;
			this.#renderThread = new Worker(
				URL.createObjectURL(new Blob([WindowThread], {type: "application/javascript"}))
			);
			this.#renderThread.postMessage({ w: width, h: height });

			let pendingFrame = false;

			this.#renderThread.onmessage = (msg)=>{
				if (msg.data == 2) {
					lib.symbols.destroy_window();
					process.exit(0);
				}
				if (pendingFrame) return;
				pendingFrame = true;
				
				
				if (msg.data == 1){
					gpuInitialized = lib.symbols.canvas_init_gpu_context();
					lib.symbols.create_window(width,height,encoder.encode(`${title}\0`))
				}
				
				lib.symbols.update_window();
				
				if (this.#wResizeViewer[0] != 0) {
					this.#wResizeViewer[0] = 0
					const details = new Event("resize",this);
					
					this.#dim[0] = this.#wResizeViewer[1]
					this.#dim[1] = this.#wResizeViewer[2]
					if (this.onresize) this.onresize(details)
						this.#rEvts.forEach((item,key)=>{
						item.cb(details);
					})
				}
				if(this.#mMoveViewer[0] != 0) {
					this.#mMoveViewer[0] = 0				
					const evt = new MouseEvent("move",
						{
							altKey:false,
							button:0,
							buttons:this.#mMoveViewer[5],
							clientX:this.#mMoveViewer[1],
							clientY:this.#mMoveViewer[2],
							movementX: this.#mMoveViewer[3],
							movementY: this.#mMoveViewer[4],
							ctrlKey:false,
							metaKey:false,
							screenX:0,
							screenY:0,
							shiftKey:false
						}
					)
					if (this.onmousemove) this.onmousemove(evt)
						this.#mMoveEvts.forEach((item,key)=>{
						item.cb(evt);
					})
				}
				if(this.#mClickViewer[0] != 0) {
					this.#mClickViewer[0] = 0
					const evt = new MouseEvent("click",
						{
							altKey:this.#mClickViewer[7],
							button:this.#mClickViewer[3],
							buttons:this.#mClickViewer[4],
							clientX:this.#mClickViewer[1],
							clientY:this.#mClickViewer[2],
							movementX: 0,
							movementY: 0,
							ctrlKey:this.#mClickViewer[6],
							metaKey:this.#mClickViewer[8],
							screenX:this.#mClickViewer[1],
							screenY:this.#mClickViewer[2],
							shiftKey:this.#mClickViewer[5]
						}
					)
					if (this.onclick) this.onclick(evt)
						this.#mClickEvts.forEach((item,key)=>{
						item.cb(evt);
					})
				}
				if(this.#kDownViewer[0] != 0) {
					this.#kDownViewer[0] = 0;
					const evt = new KeyboardEvent("down",null,this.#kDownViewer[2],this,this.#kDownViewer[1],this.#kDownViewer[3],this.#kDownViewer[4],this.#kDownViewer[5],this.#kDownViewer[6],)
					if (this.onkeydown) this.onkeydown(evt)
						this.#keyDownEvts.forEach((item,key)=>{
						item.cb(evt);
					})
				}
				
				// const pLen = requestedFrames.length
				// for (let i = 0; i < pLen; i++) {
				// 	(requestedFrames.shift())()
				// }
				setImmediate(()=>{
					const end = requestedFrames.length;

					for (let i = 0; i < end; i++) {
					    requestedFrames[i]();
					}

					requestedFrames.splice(0, end);
					pendingFrame = false;
				});
				lib.symbols.canvas_flush_gpu();
			}
		} else {
			this.#renderThread  = null;
			let cb = new JSCallback(()=>{
				if (lib.symbols.should_window_close()) {
					lib.symbols.destroy_window();
					process.exit(0);
					return
				}
				
				
				if (this.#wResizeViewer[0] != 0) {
					this.#wResizeViewer[0] = 0
					console.log("wresize!")
					const details = new Event("resize",this);
					
					this.#dim[0] = this.#wResizeViewer[1]
					this.#dim[1] = this.#wResizeViewer[2]
					if (this.onresize) {
						this.onresize(details)
					}
					this.#rEvts.forEach((item,key)=>{
							item.cb(details);
						})
				}
				if(this.#mMoveViewer[0] != 0) {
					this.#mMoveViewer[0] = 0	
					const evt = new MouseEvent("move",
						{
							target: this,
							altKey:false,
							button:0,
							buttons:this.#mMoveViewer[5],
							clientX:this.#mMoveViewer[1],
							clientY:this.#mMoveViewer[2],
							movementX: this.#mMoveViewer[3],
							movementY: this.#mMoveViewer[4],
							ctrlKey:false,
							metaKey:false,
							screenX:0,
							screenY:0,
							shiftKey:false
						}
					)
					if (this.onmousemove) {this.onmousemove(evt)}
					this.#mMoveEvts.forEach((item,key)=>{
						item.cb(evt);
					})
				}
				if(this.#mClickViewer[0] != 0) {
					this.#mClickViewer[0] = 0
					const evt = new MouseEvent("click",
						{
							altKey:this.#mClickViewer[7],
							button:this.#mClickViewer[3],
							buttons:this.#mClickViewer[4],
							clientX:this.#mClickViewer[1],
							clientY:this.#mClickViewer[2],
							movementX: 0,
							movementY: 0,
							ctrlKey:this.#mClickViewer[6],
							metaKey:this.#mClickViewer[8],
							screenX:this.#mClickViewer[1],
							screenY:this.#mClickViewer[2],
							shiftKey:this.#mClickViewer[5]
						}
					)
					if (this.onclick) this.onclick(evt)
						this.#mClickEvts.forEach((item,key)=>{
						item.cb(evt);
					})
				}
				if(this.#kDownViewer[0] != 0) {
					this.#kDownViewer[0] = 0;
					const evt = new KeyboardEvent("down",null,this.#kDownViewer[2],this,this.#kDownViewer[1],this.#kDownViewer[3],this.#kDownViewer[4],this.#kDownViewer[5],this.#kDownViewer[6],)
					if (this.onkeydown) this.onkeydown(evt)
						this.#keyDownEvts.forEach((item,key)=>{
						item.cb(evt);
					})
				}
				
				const pLen = requestedFrames.length
				for (let i = 0; i < pLen; i++) {
					(requestedFrames.shift())()
				}
			},{
				args:[],
				returns: "void"
			});
			let aaplLib = dlopen(dlPath, {
				setup_render_thread: {
					args: ["int","int","ptr"],
					returns: "void",
				}
			});
			aaplLib.symbols.setup_render_thread(width,height,cb.ptr)

			lib.symbols.create_window(width,height,encoder.encode(`${title}\0`))

			
			setInterval(()=>{
				lib.symbols.update_window(cb.ptr);
			},0);
		}
		
	}
	
	addEventListener(name : string,fn : ((event:any)=>void)){
		const pop = this.#freeIndex.pop()
		const usedIndex = pop!=undefined?pop:this.#eIndex;
		if (usedIndex == this.#eIndex) this.#eIndex++;
		
		if (name == "resize") {
			this.#rEvts.set(usedIndex, {name: name, cb: fn})
			this.#evtDetails.set(fn,usedIndex)
		} else if (name == "mousemove") {
			this.#mMoveEvts.set(usedIndex, {name: name, cb: fn})
			this.#evtDetails.set(fn,usedIndex)
		}else if (name == "click") {
			this.#mClickEvts.set(usedIndex, {name: name, cb: fn})
			this.#evtDetails.set(fn,usedIndex)
		}else if (name == "keydown") {
			this.#keyDownEvts.set(usedIndex, {name: name, cb: fn})
			this.#evtDetails.set(fn,usedIndex)
		}
	}
	removeEventListener(name : string,fn : ((event:any)=>void)){
		const det = this.#evtDetails.get(fn)
		if (name == "resize") {
			if (det != undefined) {
				this.#rEvts.delete(det)
				this.#evtDetails.delete(fn)
			}
		}else if (name == "mousemove") {
			if (det != undefined) {
				this.#mMoveEvts.delete(det)
				this.#evtDetails.delete(fn)
			}
		}else if (name == "click") {
			if (det != undefined) {
				this.#mClickEvts.delete(det)
				this.#evtDetails.delete(fn)
			}
		}else if (name == "keydown") {
			if (det != undefined) {
				this.#keyDownEvts.delete(det)
				this.#evtDetails.delete(fn)
			}
		}
	}
	
	append(canvas : Canvas){
		const canvasPtr = ptrs.get(canvas)
		if (canvasPtr == undefined) {
			console.error("Canvas was not found")
			return
		}
		lib.symbols.canvas_append(canvasPtr)
	}
	//Other Implementations to be done...
};

{
	const proto = Window.prototype;
	
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


export function requestAnimationFrame(callback :(cb : any)=>void) : void {
	requestedFrames.push(callback);
}