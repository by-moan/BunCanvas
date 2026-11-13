import { lib,encoder, dlPath } from "../internal/symbols.js";
import { Event } from "./Event.js";
import { MouseEvent } from "./MouseEvent.js"

import { dlopen, ptr, toArrayBuffer, JSCallback } from "bun:ffi"
import { KeyboardEvent } from "./KeyboardEvent.js";
import { FontFace } from "./FontFace.js";

// @ts-expect-error Suppressing missing default export for text asset import
import WindowThread from "../internal/WindowThread.js" with { type:"text" };
import { Canvas } from "./Canvas.js";
import { WheelEvent } from "./WheelEvent.js";
import { GLFW_CODE_MAP, GLFW_KEY_MAP, GLFW_KEY_MAP_SHIFT } from "../internal/window/glfwKeyMap.js";

let requestedFrames : ((arg?: number)=>void)[] = []

let render_thread : Worker;

lib.symbols.glfw_init();

if (process.platform !== "darwin"){
	render_thread = new Worker(
		URL.createObjectURL(new Blob([WindowThread], {type: "application/javascript"})),
		{ref:false}
	);
}

lib.symbols.canvas_init_gpu_context();

function attachTarget<T extends Event>(event: T, target: unknown): T {
	Object.defineProperty(event, "target", {
		value: target,
		enumerable: true,
		configurable: true,
	});
	return event;
}

export class Window {
	#rEvts = new Map();
	#mMoveEvts = new Map();
	#mDownEvts = new Map();
	#mMUpEvts = new Map();
	#mWheelEvts = new Map();
	#mClickEvts = new Map();
	#keyDownEvts = new Map();
	#keyUpEvts = new Map();

	#wPropertiesViewer = new Int32Array(toArrayBuffer(lib.symbols.get_wPropertiesViewer()!,0, 4 * 4));
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
	#getGlfwKeyLocation(kcode: number): number {
	if (kcode >= 320 && kcode <= 336) {
		return KeyboardEvent.DOM_KEY_LOCATION_NUMPAD;
	}
	if (kcode >= 340 && kcode <= 343) {
		return KeyboardEvent.DOM_KEY_LOCATION_LEFT;
	}
	if (kcode >= 344 && kcode <= 347) {
		return KeyboardEvent.DOM_KEY_LOCATION_RIGHT;
	}
	return KeyboardEvent.DOM_KEY_LOCATION_STANDARD;
	}

	#createKeyboardEvent(type: "keydown" | "keyup", viewer: ArrayLike<number>): KeyboardEvent {
	const repeat = Boolean(viewer[1]);
	const kcode = viewer[2] as keyof typeof GLFW_CODE_MAP;
	const shiftKey = Boolean(viewer[3]);
	const ctrlKey = Boolean(viewer[4]);
	const altKey = Boolean(viewer[5]);
	const metaKey = Boolean(viewer[6]);

	const key = shiftKey
		? (GLFW_KEY_MAP_SHIFT[kcode] ?? "Unidentified")
		: (GLFW_KEY_MAP[kcode] ?? "Unidentified");
	const code = GLFW_CODE_MAP[kcode] ?? "";

	return attachTarget(
		new KeyboardEvent(type, {
			key,
			code,
			location: this.#getGlfwKeyLocation(Number(kcode)),
			repeat,
			shiftKey,
			ctrlKey,
			altKey,
			metaKey,
			bubbles: true,
			cancelable: true,
		}),
		this
	);
	}

	#processEvents() {
		if (this.#wPropertiesViewer[0] !== 0) {
			this.#wPropertiesViewer[0] = 0;
			const details = attachTarget(new Event("resize"), this);

			if (this.onresize) this.onresize(details);
			this.#rEvts.forEach((item) => item.cb(details));
		}

		if (this.#mMoveViewer[0] !== 0) {
			this.#mMoveViewer[0] = 0;
			const evt = attachTarget(
			new MouseEvent("mousemove", {
				button: 0,
				buttons: this.#mMoveViewer[5],
				clientX: this.#mMoveViewer[1],
				clientY: this.#mMoveViewer[2],
				movementX: this.#mMoveViewer[3],
				movementY: this.#mMoveViewer[4],
				screenX: this.#mMoveViewer[1],
				screenY: this.#mMoveViewer[2],
				shiftKey: Boolean(this.#mMoveViewer[6]),
				ctrlKey: Boolean(this.#mMoveViewer[7]),
				altKey: Boolean(this.#mMoveViewer[8]),
				metaKey: Boolean(this.#mMoveViewer[9]),
				bubbles: true,
				cancelable: true,
			}),
			this
			);

			if (this.onmousemove) this.onmousemove(evt);
			this.#mMoveEvts.forEach((item) => item.cb(evt));
		}

		if (this.#mDownViewer[0] !== 0) {
			this.#mDownViewer[0] = 0;
			const evt = attachTarget(
			new MouseEvent("mousedown", {
				clientX: this.#mDownViewer[1],
				clientY: this.#mDownViewer[2],
				button: this.#mDownViewer[3],
				buttons: this.#mDownViewer[4],
				shiftKey: Boolean(this.#mDownViewer[5]),
				ctrlKey: Boolean(this.#mDownViewer[6]),
				altKey: Boolean(this.#mDownViewer[7]),
				metaKey: Boolean(this.#mDownViewer[8]),
				screenX: this.#mDownViewer[1],
				screenY: this.#mDownViewer[2],
				bubbles: true,
				cancelable: true,
			}),
			this
			);

			if (this.onmousedown) this.onmousedown(evt);
			this.#mDownEvts.forEach((item) => item.cb(evt));
		}

		if (this.#mUpViewer[0] !== 0) {
			this.#mUpViewer[0] = 0;
			const evt = attachTarget(
			new MouseEvent("mouseup", {
				clientX: this.#mUpViewer[1],
				clientY: this.#mUpViewer[2],
				button: this.#mUpViewer[3],
				buttons: this.#mUpViewer[4],
				shiftKey: Boolean(this.#mUpViewer[5]),
				ctrlKey: Boolean(this.#mUpViewer[6]),
				altKey: Boolean(this.#mUpViewer[7]),
				metaKey: Boolean(this.#mUpViewer[8]),
				screenX: this.#mUpViewer[1],
				screenY: this.#mUpViewer[2],
				bubbles: true,
				cancelable: true,
			}),
			this
			);

			if (this.onmouseup) this.onmouseup(evt);
			this.#mMUpEvts.forEach((item) => item.cb(evt));
		}

		if (this.#mClickViewer[0] !== 0) {
			this.#mClickViewer[0] = 0;
			const evt = attachTarget(
			new MouseEvent("click", {
				clientX: this.#mClickViewer[1],
				clientY: this.#mClickViewer[2],
				button: this.#mClickViewer[3],
				buttons: this.#mClickViewer[4],
				shiftKey: Boolean(this.#mClickViewer[5]),
				ctrlKey: Boolean(this.#mClickViewer[6]),
				altKey: Boolean(this.#mClickViewer[7]),
				metaKey: Boolean(this.#mClickViewer[8]),
				screenX: this.#mClickViewer[1],
				screenY: this.#mClickViewer[2],
				detail: 1,
				bubbles: true,
				cancelable: true,
			}),
			this
			);

			if (this.onclick) this.onclick(evt);
			this.#mClickEvts.forEach((item) => item.cb(evt));
		}

		if (this.#mWheelViewer[0] !== 0) {
			this.#mWheelViewer[0] = 0;
			const evt = attachTarget(
			new WheelEvent("wheel", {
				deltaX: -this.#mWheelViewer[1] * 120,
				deltaY: -this.#mWheelViewer[2] * 120,
				deltaMode: WheelEvent.DOM_DELTA_PIXEL,
				button: this.#mWheelViewer[3],
				buttons: this.#mWheelViewer[4],
				// Use current cursor position from #mMoveViewer[1]/[2] (indices [3]/[4] were movementX/Y)
				clientX: this.#mMoveViewer[1],
				clientY: this.#mMoveViewer[2],
				screenX: this.#mMoveViewer[1],
				screenY: this.#mMoveViewer[2],
				shiftKey: Boolean(this.#mMoveViewer[6]),
				ctrlKey: Boolean(this.#mMoveViewer[7]),
				altKey: Boolean(this.#mMoveViewer[8]),
				metaKey: Boolean(this.#mMoveViewer[9]),
				bubbles: true,
				cancelable: true,
			}),
			this
			);

			if (this.onwheel) this.onwheel(evt);
			this.#mWheelEvts.forEach((item) => item.cb(evt));
		}

		if (this.#kDownViewer[0] !== 0) {
			this.#kDownViewer[0] = 0;
			const evt = this.#createKeyboardEvent("keydown", this.#kDownViewer);

			if (this.onkeydown) this.onkeydown(evt);
			this.#keyDownEvts.forEach((item) => item.cb(evt));
		}

		if (this.#kUpViewer[0] !== 0) {
			this.#kUpViewer[0] = 0;
			const evt = this.#createKeyboardEvent("keyup", this.#kUpViewer);

			if (this.onkeyup) this.onkeyup(evt);
			this.#keyUpEvts.forEach((item) => item.cb(evt));
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
	onkeyup : ((evt: KeyboardEvent) => any) | null = null;
	
	onwheel : ((evt: MouseEvent) => any) | null = null;


	get innerWidth(){
		return this.#wPropertiesViewer[1]
	}
	get innerHeight(){
		return this.#wPropertiesViewer[2]
	}

	constructor(width : number, height : number, title = "App", args : {vsync:boolean, maxFramerate:number}) {
		if (process.platform !== "darwin") (render_thread as any).ref?.();
		this.#wPropertiesViewer[1] = width;
		this.#wPropertiesViewer[2] = height;

		const { vsync = true, maxFramerate = 0 } = args ?? {};
		this.#vsync = vsync;
		this.maxFramerate = maxFramerate;
		
		if (process.platform !== "darwin") {
			render_thread.onerror = (msg) => {
				console.error("Render thread error:", msg);
			};

			// Pre-encode string buffer once to avoid runtime allocations
			const titleBuf = encoder.encode(`${title}\0`);

			render_thread.postMessage({ 
				w: width, 
				h: height, 
				t: titleBuf, 
				vsync: this.#vsync, 
				nativePath: dlPath 
			});

			let pendingFrame = false;
			let shuttingDown = false;
			let lastFrame = performance.now();

			// Removed 'async' wrapper to prevent promise allocation overhead
			render_thread.onmessage = (msg) => {
				const frameSignal = msg.data;

				if (frameSignal == 2) {
					if (shuttingDown) return;
					shuttingDown = true;
					lib.symbols.destroy_window();
					lib.symbols.images_cleanup();
					process.exit(0);
					return;
				}

				if (frameSignal === 1) {
					lib.symbols.create_window(width, height, titleBuf);
				}
			}
			const loop = () => {
				lib.symbols.signal_frame_ready();

				const frameInterval = this.maxFramerate > 0 ? 1000 / this.maxFramerate : 0;
				const now = performance.now();
				const delta = now - lastFrame;

				// Process frame if maxFramerate is uncapped, vsync is on, or interval elapsed
				if (frameInterval === 0 || this.#vsync || delta >= frameInterval) {
					if (frameInterval > 0) {
						lastFrame = now - (delta % frameInterval);
					}

					if (!pendingFrame) {
						pendingFrame = true;

						setImmediate(() => {
							const queueLen = requestedFrames.length;
							if (queueLen > 0) {
								// Atomic swap avoids .splice() O(N) array re-indexing
								const framesToProcess = requestedFrames;
								requestedFrames = [];
								for (let i = 0; i < queueLen; i++) {
									framesToProcess[i](now);
								}
							}
							lib.symbols.canvas_flush_gpu();
							pendingFrame = false;
						});
					}
				}

				this.#processEvents();
			};

			setInterval(loop,0)
		}else /* MACOS JS LOOP */ {
			let aaplLib = dlopen(dlPath, {
				setup_render_thread: {
					args: ["int", "int", "cstring", "ptr", "bool"],
					returns: "void",
				}
			});

			const titleBuf = encoder.encode(`${title}\0`);
			aaplLib.symbols.setup_render_thread(width, height, titleBuf, 0, vsync);
			lib.symbols.create_window(width, height, titleBuf);

			const frameInterval = this.maxFramerate > 0 ? 1000 / this.maxFramerate : 0;
			let lastFrameTime = performance.now();

			const loop = () =>{
				if (this.#wPropertiesViewer[3] == 1) {
					lib.symbols.destroy_window();
					process.exit(0);
					return;
				}

				const now = performance.now();
				const delta = now - lastFrameTime;

				if (frameInterval === 0 || delta >= frameInterval) {
					lastFrameTime = frameInterval > 0 ? now - (delta % frameInterval) : now;

					this.#processEvents();
					if (requestedFrames.length > 0) {
						const framesToProcess = requestedFrames;
						requestedFrames = [];
						for (let i = 0; i < framesToProcess.length; i++) {
							framesToProcess[i](now);
						}
					}

					lib.symbols.update_window();
				}
				setImmediate(loop);
			};

			loop();
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
		}else if (name == "keyup") {
			this.#keyUpEvts.set(usedIndex, {name: name, cb: fn})
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
		}else if (name == "mousedown") {
			if (det != undefined) {
				this.#mDownEvts.delete(det)
				this.#evtDetails.delete(fn)
			}
		}else if (name == "mouseup") {
			if (det != undefined) {
				this.#mMUpEvts.delete(det)
				this.#evtDetails.delete(fn)
			}
		}else if (name == "wheel") {
			if (det != undefined) {
				this.#mWheelEvts.delete(det)
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
		}else if (name == "keyup") {
			if (det != undefined) {
				this.#keyUpEvts.delete(det)
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

