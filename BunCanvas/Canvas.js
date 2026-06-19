import lib, { encoder } from "./symbols";

class CanvasRenderingContext2D {
    set fillStyle(style) {
        lib.symbols.canvas_set_fill_style(encoder.encode(`${style}\0`))
    }
    set strokeStyle(style) {
        lib.symbols.canvas_set_stroke_style(encoder.encode(`${style}\0`))
    }

    fillRect(x,y,w,h){
        lib.symbols.canvas_fill_rect(x,y,w,h)
    }
    clearRect(x,y,w,h){
        lib.symbols.canvas_clear_rect(x,y,w,h)
    }
}
export class Canvas {
    constructor(w,h) {
    }

    getContext(contextType) {
        return new CanvasRenderingContext2D()
    }
}