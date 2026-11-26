import { lib } from "../internal/symbols.js";
import { INTERNAL_TOKEN } from "../internal/ConstructEnforcer.js";
import { CanvasBatchedRenderingContext2D, CanvasImmediateRenderingContext2D, CanvasRenderingContext2D, ctx2ds } from "./Canvas/RenderingContexts/RenderingContext2D.js";


const canvasFinalizationRegistry = new FinalizationRegistry((ptr: any)=>{
    lib.symbols.canvas_destroy(ptr);
});




interface CanvasRenderingContext2DSettings {
    alpha?: boolean;
    colorSpace?: PredefinedColorSpace;
    desynchronized?: boolean;
    willReadFrequently?: boolean;
    pipeline?: "batched" | "immediate";
}

export class Canvas {
	#dim = new Float64Array(2);

	#resizequeued = false;

	#resizeQueueCb(){
		if (this.#ctx) {
			// @ts-expect-error Supress error on non-callable method on public interface
			this.#ctx.reset(INTERNAL_TOKEN, this.#dim[0], this.#dim[1])
		}
		this.#resizequeued = false
	}
	#resizeQueueCbBound = ()=>this.#resizeQueueCb()

	#ctx : CanvasRenderingContext2D | null = null;
	constructor(w=1,h=1) {
		this.#dim[0] = w;
		this.#dim[1] = h;
	}
	
	set width(v : number){
		this.#dim[0] = v<1?1:v;
		if (!this.#resizequeued) {
			this.#resizequeued = true;
			queueMicrotask(this.#resizeQueueCbBound)
		}
	}
	set height(v : number){
		this.#dim[1] = v<1?1:v
		if (!this.#resizequeued) {
			this.#resizequeued = true;
			queueMicrotask(this.#resizeQueueCbBound)
		}
	}
	get width() : number{
		return this.#dim[0]
	}
	get height() : number {
		return this.#dim[1]
	}
	
	getContext(contextType : "2d", args : CanvasRenderingContext2DSettings) {
		if (contextType == "2d") {
			if (this.#ctx) return this.#ctx
			
			if (!args || !args?.pipeline || args?.pipeline == "batched"){
				this.#ctx = new CanvasBatchedRenderingContext2D(this.#dim[0],this.#dim[0],this, args, INTERNAL_TOKEN)
			}else if (args?.pipeline == "immediate") {
				this.#ctx = new CanvasImmediateRenderingContext2D(this.#dim[0],this.#dim[0],this, args, INTERNAL_TOKEN)
			}else {
				throw new SyntaxError(`Failed to execute 'getContext' on '${this.constructor.name}': unknown pipeline: ${args?.pipeline}`)
			}
			if(!ctx2ds.has(this)) ctx2ds.set(this,this.#ctx!)
			return this.#ctx
		}
	}

	getBoundingClientRect(){
		return {
			x: 0,
			y: 0,
			width: this.#dim[0],
			height: this.#dim[1],
			top: 0,
			bottom: this.#dim[1],
			left: 0,
			right: this.#dim[0],
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
