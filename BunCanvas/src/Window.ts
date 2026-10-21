import { lib,encoder, dlPath } from "../internal/symbols.js";
import { Event } from "./Event.js";
import { MouseEvent } from "./MouseEvent.js"

import { dlopen, ptr, toArrayBuffer, JSCallback } from "bun:ffi"
import { KeyboardEvent } from "./KeyboardEvent.js";
import { FontFace } from "./FontFace.js";

import WindowThread from "../internal/WindowThread.js" with { type:"text" };
import { Canvas } from "./Canvas.js";
import { WheelEvent } from "./WheelEvent.js";

let requestedFrames : ((arg?: number)=>void)[] = []

let render_thread : Worker;

lib.symbols.glfw_init();

if (process.platform !== "darwin"){
	render_thread = new Worker(
		URL.createObjectURL(new Blob([WindowThread], {type: "application/javascript"}))
	);
}

lib.symbols.canvas_init_gpu_context();

let windowWasCreated = false;
export class Window {
	#dim = new Float64Array(2);
	
	#rEvts = new Map();
	#mMoveEvts = new Map();
	#mDownEvts = new Map();
	#mMUpEvts = new Map();
	#mWheelEvts = new Map();
	#mClickEvts = new Map();
	#keyDownEvts = new Map();

	#wResizeViewer = new Int32Array(toArrayBuffer(lib.symbols.get_wResizeViewer()!,0, 3 * 4));
	#mMoveViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mMoveViewer()!,0, 9 * 8));
	#mClickViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mClickViewer()!,0, 9 * 8));
	#mDownViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mDownViewer()!,0, 9 * 8));
	#mUpViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mUpViewer()!,0, 9 * 8));
	#kDownViewer = new Int32Array(toArrayBuffer(lib.symbols.get_kDownViewer()!,0, 7 * 4));
	#kUpViewer = new Int32Array(toArrayBuffer(lib.symbols.get_kUpViewer()!,0, 7 * 4));
	
	#mWheelViewer = new Float64Array(toArrayBuffer(lib.symbols.get_mWheelViewer()!,0, 5 * 8));
	
	#evtDetails = new WeakMap();
	
	#freeIndex = new Array();
	#eIndex = 0

	#processEvents(){
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
					target:this,
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
		if(this.#mMoveViewer[0] != 0) {
			this.#mMoveViewer[0] = 0				
			const evt = new MouseEvent("move",
				{
					target:this,
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
		if(this.#mDownViewer[0] != 0) {
			this.#mDownViewer[0] = 0
			const evt = new MouseEvent("click",
				{
					target:this,
					altKey:this.#mDownViewer[7],
					button:this.#mDownViewer[3],
					buttons:this.#mDownViewer[4],
					clientX:this.#mDownViewer[1],
					clientY:this.#mDownViewer[2],
					movementX: 0,
					movementY: 0,
					ctrlKey:this.#mDownViewer[6],
					metaKey:this.#mDownViewer[8],
					screenX:this.#mDownViewer[1],
					screenY:this.#mDownViewer[2],
					shiftKey:this.#mDownViewer[5]
				}
			)
			if (this.onmousedown) this.onmousedown(evt);

			this.#mDownEvts.forEach((item,key)=>{
				item.cb(evt);
			})
		}
		if(this.#mUpViewer[0] != 0) {
			this.#mUpViewer[0] = 0
			const evt = new MouseEvent("click",
				{
					target:this,
					altKey:this.#mUpViewer[7],
					button:this.#mUpViewer[3],
					buttons:this.#mUpViewer[4],
					clientX:this.#mUpViewer[1],
					clientY:this.#mUpViewer[2],
					movementX: 0,
					movementY: 0,
					ctrlKey:this.#mUpViewer[6],
					metaKey:this.#mUpViewer[8],
					screenX:this.#mUpViewer[1],
					screenY:this.#mUpViewer[2],
					shiftKey:this.#mUpViewer[5]
				}
			)
			if (this.onmouseup) this.onmouseup(evt);

			this.#mMUpEvts.forEach((item,key)=>{
				item.cb(evt);
			})
		}
		if(this.#mClickViewer[0] != 0) {
			this.#mClickViewer[0] = 0
			const evt = new MouseEvent("click",
				{
					target:this,
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

		if(this.#mWheelViewer[0] != 0) {
			this.#mWheelViewer[0] = 0
			const evt = new WheelEvent("click",
				{
					target:this,
					altKey:this.#mMoveViewer[7],
					button:this.#mWheelViewer[3],
					buttons:this.#mWheelViewer[4],
					clientX:this.#mMoveViewer[3],
					clientY:this.#mMoveViewer[4],
					deltaX: -this.#mWheelViewer[1]*120,
					deltaY: -this.#mWheelViewer[2]*120,
					movementX: 0,
					movementY: 0,
					ctrlKey:this.#mMoveViewer[6],
					metaKey:this.#mMoveViewer[8],
					screenX:this.#mMoveViewer[3],
					screenY:this.#mMoveViewer[4],
					shiftKey:this.#mMoveViewer[5]
				}
			)
			if (this.onwheel) this.onwheel(evt)

			this.#mWheelEvts.forEach((item,key)=>{
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
	}

	maxFramerate = 0

	#vsync = true;
	get vsync(){return this.#vsync}
	set vsync(v : boolean){
		this.#vsync = v
		lib.symbols.set_vsync(v)
	}

	fonts = {
		add(font : FontFace){
			if (!font._nativePtr) throw new TypeError("Font was not loaded");
			lib.symbols.font_add_to_registry(encoder.encode(`${font.family}\0`),font._nativePtr)
		}
	}
	
	onresize : ((this: Window, evt: Event) => any) | null = null;
	onmousemove : ((evt: MouseEvent) => any) | null = null;
	onmousedown : ((evt: MouseEvent) => any) | null = null;
	onmouseup : ((evt: MouseEvent) => any) | null = null;
	onclick : ((evt: MouseEvent) => any) | null = null;
	onkeydown : ((evt: KeyboardEvent) => any) | null = null;
	
	onwheel : ((evt: MouseEvent) => any) | null = null;


	get innerWidth(){
		return this.#dim[0]
	}
	get innerHeight(){
		return this.#dim[1]
	}

	constructor(width : number, height : number, title = "App", args : any) {
		windowWasCreated = true;
		this.#dim[0] = width;
		this.#dim[1] = height;

		const { vsync = true, maxFramerate = 0 } = args ?? {};
		this.#vsync = vsync;
		this.maxFramerate = maxFramerate;
		let lastFrame = performance.now();

		
		if (process.platform !== "darwin") {
			render_thread.onerror = (msg)=>{
				console.log(msg)
			}
			render_thread.postMessage({ w: width, h: height, t: encoder.encode(`${title}\0`), vsync: this.#vsync, nativePath : dlPath});

			let pendingFrame = false;
			

			render_thread.onmessage = async(msg)=>{
				const frameTime = 1000 / this.maxFramerate;
				// lib.symbols.update_window();

				if (msg.data == 2) {
					lib.symbols.destroy_window();
					lib.symbols.images_cleanup();
					process.exit(0);
				}
				if (msg.data == 1){
					lib.symbols.create_window(width,height,encoder.encode(`${title}\0`))
				}
				lib.symbols.signal_frame_ready();

				if (!pendingFrame) {
					lib.symbols.canvas_flush_gpu();
					pendingFrame = true;
					setImmediate(async ()=>{
						const end = requestedFrames.length;
					
						for (let i = 0; i < end; i++) {
						    requestedFrames[i](performance.now());
						}
						// lib.symbols.signal_frame_ready();
					
						requestedFrames.splice(0, end);
					});
				}
				this.#processEvents()

				const now = performance.now();
				if (now - lastFrame >= frameTime) {
				    lastFrame += frameTime;
				    // Prevent drift if we fell far behind.
				    if (now - lastFrame >= frameTime)
				        lastFrame = now;
					pendingFrame = false;
				}else if (frameTime == Infinity) {
					pendingFrame = false;
				}else if (this.#vsync) {
					pendingFrame = false;
				}
				
			}
		} else /* MACOS JS LOOP */ {
			let sleeping = false;
			let cb = new JSCallback(async ()=>{
				if (sleeping) return;
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
					(requestedFrames.shift()!)(performance.now())
				}
				if (this.maxFramerate > 0) {
					sleeping = true;
					setTimeout(()=>{sleeping = false},Math.round(1000/this.maxFramerate-1))
				}
			},{
				args:[],
				returns: "void"
			});
			let aaplLib = dlopen(dlPath, {
				setup_render_thread: {
					args: ["int","int","cstring","ptr","bool"],
					returns: "void",
				}
			});
			aaplLib.symbols.setup_render_thread(width,height,encoder.encode(`${title}\0`),cb.ptr,vsync)

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
		}else if (name == "mousedown") {
			this.#mDownEvts.set(usedIndex, {name: name, cb: fn})
			this.#evtDetails.set(fn,usedIndex)
		}else if (name == "mouseup") {
			this.#mMUpEvts.set(usedIndex, {name: name, cb: fn})
			this.#evtDetails.set(fn,usedIndex)
		}else if (name == "wheel") {
			this.#mWheelEvts.set(usedIndex, {name: name, cb: fn})
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
		const canvasPtr = canvas._ptr
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


let rAf;
if (globalThis.requestAnimationFrame!) {
	rAf = globalThis.requestAnimationFrame
}else {
	rAf = function requestAnimationFrame(callback :(cb : any)=>void) : void {
		setImmediate(()=>{requestedFrames.push(callback);})
	}
	rAf.toString = () =>
		`function requestAnimationFrame() { [native code] }`;
}

export const requestAnimationFrame = rAf

