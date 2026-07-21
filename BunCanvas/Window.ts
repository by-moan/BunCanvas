import { lib,encoder, dlPath } from "./symbols.js";
import { Event } from "./Event.js";
import { MouseEvent } from "./MouseEvent.js"

import { dlopen, ptr, toArrayBuffer, JSCallback } from "bun:ffi"
import { KeyboardEvent } from "./KeyboardEvent";
import { FontFace } from "./FontFace.js";

import WindowThread from "./WindowThread.js" with { type:"text" };
import { Canvas } from "./BunCanvas.js";

let requestedFrames : any[] = []


export class Window {
	#dim = new Float64Array(2);
	
	#rEvts = new Map();
	#mMoveEvts = new Map();
	#mClickEvts = new Map();
	#keyDownEvts = new Map();
	
	#evtDetails = new WeakMap();
	
	#freeIndex = new Array();
	#eIndex = 0

	maxFramerate = 0

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
	
	constructor(width : number, height : number, title = "App", args : any) {
		this.#dim[0] = width;
		this.#dim[1] = height;

		const { vsync = true} = args ?? {};

		this.maxFramerate = args.maxFramerate?args.maxFramerate:0;
		
		if (process.platform !== "darwin") {
			let gpuInitialized = false;
			this.#renderThread = new Worker(
				URL.createObjectURL(new Blob([WindowThread], {type: "application/javascript"}))
			);
			this.#renderThread.onerror = (msg)=>{
				console.log(msg)
			}
			this.#renderThread.postMessage({ w: width, h: height, t: encoder.encode(`${title}\0`), vsync: vsync});

			let pendingFrame = false;

			this.#renderThread.onmessage = (msg)=>{
				if (msg.data == 2) {
					lib.symbols.destroy_window();
					process.exit(0);
				}
				if (msg.data == 1){
					gpuInitialized = lib.symbols.canvas_init_gpu_context();
					lib.symbols.create_window(width,height,encoder.encode(`${title}\0`))
				}
				if (pendingFrame) return;
				pendingFrame = true;
				setImmediate(()=>{
					const end = requestedFrames.length;

					// for (let i = 0; i < end; i++) {
					//     requestedFrames[i]();
					// }
					requestedFrames.forEach((item)=>{
						item();
					});
					lib.symbols.canvas_flush_gpu();
					lib.symbols.signal_frame_ready();
					requestedFrames.splice(0, end);
					pendingFrame = false;
				});
				
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
				
				
			}
		} else {
			this.#renderThread  = null;
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
					(requestedFrames.shift())()
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

export function requestAnimationFrame(callback :(cb : any)=>void) : void {
	requestedFrames.push(callback);
}

requestAnimationFrame.toString = () =>
`function requestAnimationFrame() { [native code] }`;