import { isValidColor } from "../../internal/colorCheck";
import { INTERNAL_TOKEN, throwIllegalConstructor } from "../../internal/ConstructEnforcer";
import { GradientType } from "../../internal/native/enums";
import { encodeCString } from "../../internal/symbols";

export class CanvasGradient {
    type : GradientType;
    points = new Float32Array(6);
    offsets: number[] = [];
    colors: Uint8Array[] = [];


    /*Linear
        points[0] p0.x
        points[1] p0.y
        points[2] p1.x
        points[3] p1.y
    */
    
    /*Radial
        points[0] c0.x
        points[1] c0.y
        points[2] c1.x
        points[3] c1.y
        points[4] r0
        points[5] r1
    */
    
    /*Conic
        points[0] center.x
        points[1] center.y
        points[2] angle
    */
    
    constructor(token: {}, type : GradientType, a : number, x1 : number, y1 : number);
    constructor(token: {}, type : GradientType, x1 : number, y1 : number, x2 : number, y2 : number);
    constructor(token: {}, type : GradientType, x1 : number, y1 : number, x2 : number, y2 : number, r0 : number, r1 : number);
    constructor(...args : any[]){
        if (args[0] !== INTERNAL_TOKEN) throwIllegalConstructor(this)
        this.type = args[1]
        switch (this.type) {
            case GradientType.Linear: {
                this.points[0] = args[2]
                this.points[1] = args[3]
                this.points[2] = args[4]
                this.points[3] = args[5]
                break;
            }
            case GradientType.Radial: {
                this.points[0] = args[2]
                this.points[1] = args[3]
                this.points[2] = args[4]
                this.points[3] = args[5]
                this.points[4] = args[6]
                this.points[5] = args[7]
                break;
            }
            case GradientType.Conic: {
                this.points[0] = args[2]
                this.points[1] = args[3]
                this.points[2] = args[4]
                break;
            }
        }   
    }

    addColorStop(offset : number, color : string) {
        if (offset < 0 || offset > 1) throw new RangeError(`Failed to execute 'addColorStop' on 'CanvasGradient': The provided value (${offset}) is outside the range (0.0, 1.0).`)
        if (!isValidColor(color)) throw new SyntaxError(`Failed to execute 'addColorStop' on 'CanvasGradient': The value provided ('${color}') could not be parsed as a color.`)
        
        // Maintain sorted order on insertion
        const idx = this.offsets.findIndex(o => o > offset);
        const insertIdx = idx === -1 ? this.offsets.length : idx;

        this.offsets.splice(insertIdx, 0, offset);
        this.colors.splice(insertIdx, 0, encodeCString(color));
    }
};

{
    const proto = CanvasGradient.prototype;
    
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