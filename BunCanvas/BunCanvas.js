import { lib,encoder } from "./symbols";
import { Event } from "./Event";
import { MouseEvent } from "./MouseEvent"

import { ptr, toArrayBuffer } from "bun:ffi"
import { KeyboardEvent } from "./KeyboardEvent";
import WindowThread from "./Windowthread.txt"

const requestedFrames = []
const ptrs = new WeakMap();
let worker;

export class Image {
	constructor() {
		ptrs.set(this,lib.symbols.image_create(encoder.encode(`\0`)))
	}

	onload = ()=>{}
	onerror = ()=>{}

	set src(path){
		if (lib.symbols.image_set_src(ptrs.get(this), encoder.encode(`${path}\0`))) {
			this.onload(new Event("load",this));
		}else {
			this.onerror(new Event("error",this));
		}
	}
}

export class ImageData {
	width = 0;
	height = 0;
	data = null;
	constructor(width,height,data){
		this.width = width;
		this.height = height;
		if (data instanceof Uint8Array){
			this.data = data;
		}
	}
}

class CanvasRenderingContext2D {
    #iptr;
	#lineWidth;
	#compositeOperation;

    set fillStyle(style) {
        lib.symbols.canvas_set_fill_style(this.#iptr,encoder.encode(`${style}\0`))
    }
    set strokeStyle(style) {
        lib.symbols.canvas_set_stroke_style(this.#iptr,encoder.encode(`${style}\0`))
    }
    set lineWidth(width) {
        lib.symbols.canvas_set_stroke_width(this.#iptr,width)
		this.#lineWidth = width
    }
    get lineWidth() {
       	return this.#lineWidth
    }
    set globalCompositeOperation(operation) {
        if(lib.symbols.canvas_set_composite_operation(this.#iptr,encoder.encode(`${operation}\0`)) == true)
			this.#compositeOperation = operation
    }

	get globalCompositeOperation(){
		return this.#compositeOperation
	}

    fillRect(x,y,w,h){
        lib.symbols.canvas_fill_rect(this.#iptr,x,y,w,h)
    }
    clearRect(x,y,w,h){
        lib.symbols.canvas_clear_rect(this.#iptr,x,y,w,h)
    }
    beginPath(x,y,w,h){
        lib.symbols.canvas_path_begin(this.#iptr)
    }
    moveTo(x,y){
        lib.symbols.canvas_path_move_to(this.#iptr,x,y)
    }
    lineTo(x,y){
        lib.symbols.canvas_path_line_to(this.#iptr,x,y)
    }
    arc(x, y, radius, startAngle, endAngle){
        lib.symbols.canvas_path_arc(this.#iptr,x, y, radius, startAngle, endAngle)
    }
    arcTo(x1, y1, x2, y2, radius){
        lib.symbols.canvas_path_arc_to(this.#iptr,x1, y1, x2, y2, radius)
    }
    bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y){
        lib.symbols.canvas_path_bezier_to(this.#iptr,cp1x, cp1y, cp2x, cp2y, x, y)
    }
    closePath(){
        lib.symbols.canvas_path_close(this.#iptr)
	}
    stroke(){
        lib.symbols.canvas_path_stroke(this.#iptr)
    }
	drawImage(img,x,y,w,h) {
        lib.symbols.canvas_draw_image(this.#iptr,ptrs.get(img),x,y,w,h);
	}
	getImageData(x,y,w,h) {
		const arr = new Uint8Array(w*h*4);
        lib.symbols.canvas_get_image_data(this.#iptr,x,y,w,h, ptr(arr))
		return new ImageData(w,h,arr);
	}
	putImageData(data,dx,dy,dirtyX = 0, dirtyY = 0,dirtyWidth = (data instanceof ImageData)?data.width:(()=>{throw new TypeError("Failed to execute 'putImageData' on 'CanvasRenderingContext2D': parameter 1 is not of type 'ImageData'")})(), dirtyHeight = data.height) {
		if (!(data instanceof ImageData)) throw new TypeError("Failed to execute 'putImageData' on 'CanvasRenderingContext2D': parameter 1 is not of type 'ImageData'")
		lib.symbols.canvas_put_image_data(this.#iptr,dx,dy,dirtyWidth,dirtyHeight, ptr(data.data));
	}
    // clearRect(x,y,w,h){
    //     lib.symbols.canvas_clear_rect(x,y,w,h)
    // }
    constructor(iptr) {
		

		const renderContextPtr = lib.symbols.canvas_setup_context(iptr, encoder.encode(`2d\0`))
		ptrs.set(this, renderContextPtr)
        this.#iptr = renderContextPtr
        
    }
}

{
	const proto = CanvasRenderingContext2D.prototype;

	for (const key of Object.getOwnPropertyNames(proto)) {
	    const desc = Object.getOwnPropertyDescriptor(proto, key);
		if (key == "constructor"){
			desc.value.toString = () =>
	            `function ${proto.constructor.name}() { [native code] }`;
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
		return this.#dim[0]
	}
	get height(){
		return this.#dim[1]
	}

    getContext(contextType) {
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
			desc.value.toString = () =>
	            `function ${proto.constructor.name}() { [native code] }`;
			continue
		}
	    if (typeof desc?.value === "function") {
	        desc.value.toString = () =>
	            `function ${key}() { [native code] }`;
	    }
	}
}

export class Window {
	#wndPtr;

	#dim = new Float64Array(2);

	#rEvts = new Map();
	#mMoveEvts = new Map();
	#mClickEvts = new Map();
	#keyDownEvts = new Map();

	#evtDetails = new WeakMap();
	
	#freeIndex = new Array();
	#eIndex = 0

	onresize = null
	onmousemove = null
	onclick = null
	onkeydown = null

	get innerWidth(){
		return this.#dim[0]
	}
	get innerHeight(){
		return this.#dim[1]
	}

	#wResizeViewer = new Int32Array(3);
	#mMoveViewer = new Float64Array(9);
	#mClickViewer = new Float64Array(9);
	#mDownViewer = new Float64Array(9);
	#mUpViewer = new Float64Array(9);

	#kDownViewer = new Int32Array(7);
	#kUpViewer = new Int32Array(7);

	constructor(width, height, title = "App") {
		lib.symbols.create_window(width,height,encoder.encode(`${title}\0`),
			ptr(this.#wResizeViewer),
			ptr(this.#mMoveViewer),
			ptr(this.#mClickViewer),
			ptr(this.#mDownViewer),
			ptr(this.#mUpViewer),
			ptr(this.#kDownViewer),
			ptr(this.#kUpViewer),
		)
		this.#dim[0] = width;
		this.#dim[1] = height;
		let loop = ()=>{
			if (lib.symbols.should_window_close()) {
				console.log("exit")
				lib.symbols.destroy_window();
				process.exit(0);
				return
			}
			
			lib.symbols.update_window(
				ptr(this.#wResizeViewer),
				ptr(this.#mMoveViewer),
				ptr(this.#mClickViewer),
				ptr(this.#mDownViewer),
				ptr(this.#mUpViewer),
				ptr(this.#kDownViewer),
				ptr(this.#kUpViewer),
			);

			if (this.#wResizeViewer[0] != 0) {
				this.#wResizeViewer[0] = 0
				// const details = {type: "resize", target: this, timestamp: performance.now()};
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
				this.#kDownViewer[0] = false;
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
			setTimeout(loop,0)
		}

		worker = new Worker(URL.createObjectURL(new Blob([WindowThread], {
			  type: "application/javascript",
			}))
		);
		worker.onerror = ()=>{
			console.error("error loading worker!")
		}
		worker.onmessage = (msg)=>{
			console.log(msg)
		}
		loop()
	}

	addEventListener(name,fn){
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
	removeEventListener(name,fn){
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

	append(canvas){
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
			desc.value.toString = () =>
	            `function ${proto.constructor.name}() { [native code] }`;
			continue
		}
	    if (typeof desc?.value === "function") {
	        desc.value.toString = () =>
	            `function ${key}() { [native code] }`;
	    }
	}
}


export function requestAnimationFrame(callback) {
	requestedFrames.push(callback);
}