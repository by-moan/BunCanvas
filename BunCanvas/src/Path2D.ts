import { lib } from "../internal/symbols.js";
import { ptr, Pointer } from "bun:ffi"

const p2dFinalizationRegistry = new FinalizationRegistry((ptr: any)=>{
    lib.symbols.path2d_destroy(ptr);
});

const ptrs = new WeakMap();

export class Path2D {
        constructor(path : Path2D);
        constructor(path : Path2D | undefined){
            if (path && path instanceof Path2D) {
                let _ptr = lib.symbols.path2d_create_from_copy(path.ptr);
                ptrs.set(this,_ptr);
                p2dFinalizationRegistry.register(this,_ptr)

            }else{
                let _ptr = lib.symbols.path2d_create()
                ptrs.set(this,_ptr)
                p2dFinalizationRegistry.register(this,_ptr)
            }
        }
        moveTo(x : number,y : number){
            lib.symbols.path2d_move_to(ptrs.get(this),x,y)
        }
        lineTo(x : number,y : number){
            lib.symbols.path2d_line_to(ptrs.get(this),x,y)
        }
        arc(x : number, y : number, radius : number, startAngle : number, endAngle : number){
            lib.symbols.path2d_arc(ptrs.get(this),x, y, radius, startAngle, endAngle)
        }
        ellipse(x : number, y : number, radiusX : number, radiusY : number, rotation : number, startAngle : number, endAngle : number, counterclockwise : boolean = false){
            lib.symbols.path2d_ellipse(ptrs.get(this),x, y, radiusX, radiusY, rotation, startAngle, endAngle, counterclockwise)
        }
        arcTo(x1 : number, y1 : number, x2 : number, y2 : number, radius : number){
            lib.symbols.path2d_arc_to(ptrs.get(this),x1, y1, x2, y2, radius)
        }
        bezierCurveTo(cp1x : number, cp1y : number, cp2x : number, cp2y : number, x : number, y : number){
            lib.symbols.path2d_bezier_to(ptrs.get(this),cp1x, cp1y, cp2x, cp2y, x, y)
        }
        quadraticCurveTo(cpx : number, cpy : number, x : number, y : number){
            lib.symbols.path2d_quad_to(ptrs.get(this),cpx,cpy, x, y)
        }
        rect(x : number, y : number, width : number, height : number){
            lib.symbols.path2d_rect(ptrs.get(this),x,y,width,height)
        }
        roundRect(x : number, y : number, width : number, height : number, radii: any){
            if (radii.constructor.name.toLowerCase().includes("array")){
                const arr = new Float32Array(radii);
                if(arr.some(Number.isNaN)) return;
                const len =arr.length;
                lib.symbols.path2d_roundrect_array(ptrs.get(this),x,y,width,height,ptr(arr), len)
                return;
            }else {
                lib.symbols.path2d_rect(ptrs.get(this),x,y,width,height,radii)
            }
        }
        closePath(){
            lib.symbols.path2d_close(ptrs.get(this))
        }

        get ptr(){
            return ptrs.get(this)
        }
    };

    const proto = Path2D.prototype;
        
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