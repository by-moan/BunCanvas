import { lib,encoder } from "./symbols";

const requestedFrames = []
const canvasPtrs = new WeakMap();

class CanvasRenderingContext2D {
    #iptr;

    set fillStyle(style) {
        lib.symbols.canvas_set_fill_style(this.#iptr,encoder.encode(`${style}\0`))
    }
    // set strokeStyle(style) {
    //     lib.symbols.canvas_set_stroke_style(encoder.encode(`${style}\0`))
    // }

    fillRect(x,y,w,h){
        lib.symbols.canvas_fill_rect(this.#iptr,x,y,w,h)
    }
    // clearRect(x,y,w,h){
    //     lib.symbols.canvas_clear_rect(x,y,w,h)
    // }
    constructor(iptr) {
        this.#iptr = iptr
        lib.symbols.canvas_setup_context(this.#iptr)
    }
}

export class Canvas {
    #iptr;
    constructor(w=1,h=1) {
        this.#iptr = lib.symbols.canvas_create(w,h)
		canvasPtrs.set(this,this.#iptr)
    }

    getContext(contextType) {
        return new CanvasRenderingContext2D(this.#iptr)
    }
}

export class Window {
	#wndPtr;

	#dim = new Float64Array(2);

	#rEvts = new Map();
	#evtDetails = new WeakMap();
	
	#freeIndex = new Array();
	#eIndex = 0
	onresize = ()=>{}

	get innerWidth(){
		return this.#dim[0]
	}
	get innerHeight(){
		return this.#dim[1]
	}

	#lastMouseX = 0
	#lastMouseY = 0

	constructor(width, height) {
		lib.symbols.create_window(width,height)
		
		let lp = ()=>{
			if (lib.symbols.should_window_close()) {
				lib.symbols.destroy_window();
				process.exit(0);
				return
			}
			
			lib.symbols.update_window();

			const evts = JSON.parse(lib.symbols.window_query_events())

			for (const event of evts) {
				if (event.type == "wresize") {
					this.#dim[0] = event.values.width
					this.#dim[1] = event.values.height

					const details = {type: "resize", target: this, timestamp: performance.now()};
					this.onresize(details)
					this.#rEvts.forEach((item,key)=>{
						item(details);
					})
				}
			}

			const pLen = requestedFrames.length
			for (let i = 0; i < pLen; i++) {
				(requestedFrames.shift())()
			}
			setTimeout(lp,0)
		}

		lp();
	}

	addEventListener(name,fn){
		const pop = this.#freeIndex.pop()
		const usedIndex = pop!=undefined?pop:this.#eIndex;
		if (usedIndex == this.#eIndex) this.#eIndex++;

		if (name == "resize") {
			this.#rEvts.set(this.#eIndex, {name: name, cb: fn})
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
		}
	}

	append(canvas){
		const ptr = canvasPtrs.get(canvas)
		if (ptr == undefined) {
			console.error("Canvas was not found")
			return
		}
		lib.symbols.canvas_append(ptr)
	}
	//Other Implementations to be done...
};

export function requestAnimationFrame(callback) {
	requestedFrames.push(callback);
}