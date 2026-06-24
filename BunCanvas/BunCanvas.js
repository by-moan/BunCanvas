import { lib,encoder } from "./symbols";
import { MouseEvent } from "./MouseEvent"
const requestedFrames = []
const ptrs = new WeakMap();


export class Image {
	constructor(path) {
		ptrs.set(this,lib.symbols.image_load(encoder.encode(`${path}\0`)))
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
        return new CanvasRenderingContext2D(this.#iptr)
    }
}

export class Window {
	#wndPtr;

	#dim = new Float64Array(2);

	#rEvts = new Map();
	#mMoveEvts = new Map();

	#evtDetails = new WeakMap();
	
	#freeIndex = new Array();
	#eIndex = 0

	onresize = ()=>{}
	onmousemove = ()=>{}

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
		
		setInterval(()=>{
			if (lib.symbols.should_window_close()) {
				lib.symbols.destroy_window();
				process.exit(0);
				return
			}
			
			lib.symbols.update_window();

			const evts = JSON.parse(lib.symbols.window_query_events())

			if (evts.length != 0) {
				for (const event of evts) {
					if (event.type == "wresize") {
						this.#dim[0] = event.values.width
						this.#dim[1] = event.values.height

						const details = {type: "resize", target: this, timestamp: performance.now()};
						this.onresize(details)
						this.#rEvts.forEach((item,key)=>{
							item.cb(details);
						})
					}
					if (event.type == "mmove") {
						const evt = new MouseEvent("move", {altKey:false, button:0, buttons:0, clientX:event.values.xpos, clientY:event.values.ypos, movementX: event.values.movx, movementY: event.values.movy, ctrlKey:false, metaKey:false, screenX:0, screenY:0, shiftKey:false})
						this.onmousemove(evt)
						this.#mMoveEvts.forEach((item,key)=>{
							item.cb(evt);
						})
					}
				}
			}

			const pLen = requestedFrames.length
			for (let i = 0; i < pLen; i++) {
				(requestedFrames.shift())()
			}

		},0)
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
		}
	}

	append(canvas){
		const ptr = ptrs.get(canvas)
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