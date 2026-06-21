import lib, { encoder } from "./symbols";

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
    __iptr;
    constructor(w=1,h=1) {
        this.__iptr = lib.symbols.canvas_create(w,h)
    }

    getContext(contextType) {
        return new CanvasRenderingContext2D(this.__iptr)
    }
}