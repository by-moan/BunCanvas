import { ptr } from "bun:ffi"

function isMatrixArray(
    value: unknown
): value is number[] | Float32Array | Float64Array {
    //value == undefined because it accepts empty values. So a default constructor without arguments is accepted 
    return value == undefined || Array.isArray(value) ||
        (ArrayBuffer.isView(value) && !(value instanceof DataView));
}

function multipliedMatrices(a : Float64Array, b : Float64Array) {
        // returns a * b
    const out = new Float64Array(16);
    out[0]  = a[0]*b[0]  + a[1]*b[4]  + a[2]*b[8]  + a[3]*b[12];
    out[1]  = a[0]*b[1]  + a[1]*b[5]  + a[2]*b[9]  + a[3]*b[13];
    out[2]  = a[0]*b[2]  + a[1]*b[6]  + a[2]*b[10] + a[3]*b[14];
    out[3]  = a[0]*b[3]  + a[1]*b[7]  + a[2]*b[11] + a[3]*b[15];

    out[4]  = a[4]*b[0]  + a[5]*b[4]  + a[6]*b[8]  + a[7]*b[12];
    out[5]  = a[4]*b[1]  + a[5]*b[5]  + a[6]*b[9]  + a[7]*b[13];
    out[6]  = a[4]*b[2]  + a[5]*b[6]  + a[6]*b[10] + a[7]*b[14];
    out[7]  = a[4]*b[3]  + a[5]*b[7]  + a[6]*b[11] + a[7]*b[15];

    out[8]  = a[8]*b[0]  + a[9]*b[4]  + a[10]*b[8] + a[11]*b[12];
    out[9]  = a[8]*b[1]  + a[9]*b[5]  + a[10]*b[9] + a[11]*b[13];
    out[10] = a[8]*b[2]  + a[9]*b[6]  + a[10]*b[10]+ a[11]*b[14];
    out[11] = a[8]*b[3]  + a[9]*b[7]  + a[10]*b[11]+ a[11]*b[15];

    out[12] = a[12]*b[0] + a[13]*b[4] + a[14]*b[8] + a[15]*b[12];
    out[13] = a[12]*b[1] + a[13]*b[5] + a[14]*b[9] + a[15]*b[13];
    out[14] = a[12]*b[2] + a[13]*b[6] + a[14]*b[10]+ a[15]*b[14];
    out[15] = a[12]*b[3] + a[13]*b[7] + a[14]*b[11]+ a[15]*b[15];

    return out;
}

function throwParseErrorDMatrix(fName : string, pString : string) {
    throw new SyntaxError(`Failed to execute '${fName}' on 'DOMMatrix': Failed to parse '${pString}'.`)
}

export class DOMMatrix {
    #internal = new Float64Array([1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1])
    #isIdentity = true;
    #computeIdentity() {
        this.#isIdentity = this.#internal[0]  == 1 && this.#internal[1]  == 0 && this.#internal[2]  == 0 && this.#internal[3]  == 0 &&
        this.#internal[4]  == 0 && this.#internal[5]  == 1 && this.#internal[6]  == 0 && this.#internal[7]  == 0 &&
        this.#internal[8]  == 0 && this.#internal[9]  == 0 && this.#internal[10] == 1 && this.#internal[11] == 0 &&
        this.#internal[12] == 0 && this.#internal[13] == 0 && this.#internal[14] == 0 && this.#internal[15] == 1
    }

    get a() : number{
        return this.m11;
        // return this.m11;
    }
    set a(v: any){
        this.m11 = v
    }
    get b() : number{
        return this.m12;
    }
    set b(v: any){
        this.m12 = v
    }
    get c() : number{
        return this.m21;
    }
    set c(v: any){
        this.m21 = v
    }
    get d() : number{
        return this.m22;
    }
    set d(v: any){
        this.m22 = v
    }
    get e() : number{
        return this.m41;
    }
    set e(v : any){
        this.m41 = v
    }
    get f() : number{
        return this.m42;
    }
    set f(v : any){
        this.m42 = v
    }
    is2D = true;
    get isIdentity() : boolean {
        return this.#isIdentity
    }
    get m11() : number{
        return this.#internal[0];
    }
    set m11(v : any){
        this.#internal[0] = v
        if (!this.#isIdentity) {
            if (v == 1.0)this.#computeIdentity();
        } else {
            if (v != 1.0)
                this.#isIdentity = false;
        }
    }
    get m12() : number{
        return this.#internal[4];
    }
    set m12(v : any){
        this.#internal[4] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m13() : number{
        return this.#internal[8];
    }
    set m13(v : any){
        this.#internal[8] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m14() : number{
        return this.#internal[12];
    }
    set m14(v : any){
        this.#internal[12] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }

    get m21() : number{
        return this.#internal[1];
    }
    set m21(v : any){
        this.#internal[1] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m22() : number{
        return this.#internal[5];
    }
    set m22(v : any){
        this.#internal[5] = v
        if (!this.#isIdentity) {
            if (v == 1.0)this.#computeIdentity();
        } else {
            if (v != 1.0)
                this.#isIdentity = false;
        }
    }
    get m23() : number{
        return this.#internal[9];
    }
    set m23(v : any){
        this.#internal[9] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m24() : number{
        return this.#internal[13];
    }
    set m24(v : any){
        this.#internal[13] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }

    get m31() : number{
        return this.#internal[2];
    }
    set m31(v : any){
        this.#internal[2] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m32() : number{
        return this.#internal[6];
    }
    set m32(v : any){
        this.#internal[6] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m33() : number{
        return this.#internal[10];
    }
    set m33(v : any){
        this.#internal[10] = v
        if (!this.#isIdentity) {
            if (v == 1.0)this.#computeIdentity();
        } else {
            if (v != 1.0)
                this.#isIdentity = false;
        }
    }
    get m34() : number{
        return this.#internal[14];
    }
    set m34(v : any){
        this.#internal[14] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }

    get m41() : number{
        return this.#internal[3];
    }
    set m41(v : any){
        this.#internal[3] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m42() : number{
        return this.#internal[7];
    }
    set m42(v : any){
        this.#internal[7] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m43() : number{
        return this.#internal[11];
    }
    set m43(v : any){
        this.#internal[11] = v
        if (!this.#isIdentity) {
            if (v == 0)this.#computeIdentity();
        } else {
            if (v != 0)
                this.#isIdentity = false;
        }
    }
    get m44() : number{
        return this.#internal[15];
    }
    set m44(v : any){
        this.#internal[15] = v
        if (!this.#isIdentity) {
            if (v == 1.0)this.#computeIdentity();
        } else {
            if (v != 1.0)
                this.#isIdentity = false;
        }
    }

    multiplySelf(other : DOMMatrix) {
        const a = this.#internal;
        const b = other.#internal;

        this.#internal.set(multipliedMatrices(a,b));

        this.is2D &&= other.is2D;

        return this;
    }

    preMultiplySelf(other : DOMMatrix) {
        const a = this.#internal;
        const b = other.#internal;

        this.#internal.set(multipliedMatrices(b,a));

        this.is2D &&= other.is2D;

        return this;
    }

    translateSelf(tx = 0, ty = 0, tz = 0) {
        const t = new Float64Array([
            1, 0, 0, tx,
            0, 1, 0, ty,
            0, 0, 1, tz,
            0, 0, 0, 1
        ]);

        this.#internal.set(multipliedMatrices(this.#internal, t));

        if (tz !== 0)
            this.is2D = false;

        return this;
    }

    scaleSelf(
        scaleX = 1,
        scaleY = scaleX,
        scaleZ = 1,
        originX = 0,
        originY = 0,
        originZ = 0
    ) {
        if (originX || originY || originZ)
            this.translateSelf(originX, originY, originZ);

        const s = new Float64Array([
            scaleX, 0,      0,      0,
            0,      scaleY, 0,      0,
            0,      0,      scaleZ, 0,
            0,      0,      0,      1
        ]);

        this.#internal.set(multipliedMatrices(this.#internal, s));

        if (originX || originY || originZ)
            this.translateSelf(-originX, -originY, -originZ);

        if (scaleZ !== 1 || originZ !== 0)
            this.is2D = false;

        return this;
    }

    scale3dSelf(scale : number, originX = 0, originY = 0, originZ = 0) {
        return this.scaleSelf(
            scale,
            scale,
            scale,
            originX,
            originY,
            originZ
        );
    }
    
    rotateSelf(rotZ : number) : DOMMatrix;
    rotateSelf(rotX : number, rotY : number) : DOMMatrix;
    rotateSelf(rotX : number, rotY: number, rotZ: number) : DOMMatrix;
    // rotateSelf(rotX = 0, rotY, rotZ) {
    rotateSelf(...args: number[]) {
        if (args[1] === undefined && args[2] === undefined) {
            args[2] = args[0];
            args[0] = 0;
            args[1] = 0;
        } else {
            args[1] ??= 0;
            args[2] ??= 0;
        }
    
        const rx = args[0] * Math.PI / 180;
        const ry = args[1] * Math.PI / 180;
        const rz = args[2] * Math.PI / 180;
        const sx = Math.sin(rx), cx = Math.cos(rx);
        const sy = Math.sin(ry), cy = Math.cos(ry);
        const sz = Math.sin(rz), cz = Math.cos(rz);
    
        if (args[2] !== 0) {
            this.#internal.set(multipliedMatrices(
                this.#internal,
                new Float64Array([
                    cz, -sz, 0, 0,
                    sz,  cz, 0, 0,
                    0,   0, 1, 0,
                    0,   0, 0, 1
                ])
            ));
        }
    
        if (args[1] !== 0) {
            this.#internal.set(multipliedMatrices(
                new Float64Array([
                     cy, 0, sy, 0,
                      0, 1,  0, 0,
                    -sy, 0, cy, 0,
                      0, 0,  0, 1
                ]),
                this.#internal
            ));
        }
    
        if (args[0] !== 0) {
            this.#internal.set(multipliedMatrices(
                new Float64Array([
                    1,  0,   0, 0,
                    0, cx, -sx, 0,
                    0, sx,  cx, 0,
                    0,  0,   0, 1
                ]),
                this.#internal
            ));
        }
    
        if (args[0] !== 0 || args[1] !== 0)
            this.is2D = false;
    
        return this;
    }

    rotateAxisAngleSelf(x = 0, y = 0, z = 1, angle = 0) {
        const length = Math.hypot(x, y, z);

        if (length === 0)
            return this;

        x /= length;
        y /= length;
        z /= length;

        const rad = angle * Math.PI / 180;

        const s = Math.sin(rad);
        const c = Math.cos(rad);
        const t = 1 - c;

        const r = new Float64Array([
            t*x*x + c,      t*x*y - s*z,    t*x*z + s*y,    0,
            t*x*y + s*z,    t*y*y + c,      t*y*z - s*x,    0,
            t*x*z - s*y,    t*y*z + s*x,    t*z*z + c,      0,
            0,              0,              0,              1
        ]);

        this.#internal.set(multipliedMatrices(this.#internal, r));

        if (x !== 0 || y !== 0)
            this.is2D = false;

        return this;
    }
    rotateFromVectorSelf(x = 0, y = 0) {
        if (x === 0 && y === 0)
            return this;

        const angle = Math.atan2(y, x) * 180 / Math.PI;

        return this.rotateSelf(angle);
    }

    skewXSelf(sx = 0) {
        const t = Math.tan(sx * Math.PI / 180);

        this.#internal.set(multipliedMatrices(
            this.#internal,
            new Float64Array([
                1, t, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            ])
        ));

        return this;
    }

    skewYSelf(sy = 0) {
        const t = Math.tan(sy * Math.PI / 180);

        this.#internal.set(multipliedMatrices(
            this.#internal,
            new Float64Array([
                1, 0, 0, 0,
                t, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            ])
        ));

        return this;
    }
    #parseTransformList(value: string) {
        const transforms = [];

        let i = 0;

        while (i < value.length) {
            while (/\s/.test(value[i])) i++;

            if (i >= value.length)
                break;

            const nameStart = i;

            while (/[A-Za-z0-9]/.test(value[i]))
                i++;

            const name = value.substring(nameStart, i);

            while (/\s/.test(value[i])) i++;

            if (value[i] !== "(")
                return "error";

            i++;

            let depth = 1;
            const argsStart = i;

            while (i < value.length && depth > 0) {
                if (value[i] === "(")
                    depth++;
                else if (value[i] === ")")
                    depth--;

                i++;
            }

            if (depth !== 0)
                return "error";

            transforms.push({
                name,
                args: value.substring(argsStart, i - 1)
            });
        }

        return transforms;
    }

    #splitArguments(args: string) {
        const result = [];

        let current = "";
        let depth = 0;

        for (const c of args) {
            if (c === "(") depth++;
            if (c === ")") depth--;

            if ((c === "," || /\s/.test(c)) && depth === 0) {
                if (current.length) {
                    result.push(current);
                    current = "";
                }
                continue;
            }

            current += c;
        }

        if (current.length)
            result.push(current);

        return result;
    }


    #parseAngle(value: string) {
        if (value.endsWith("deg"))
            return Number.parseFloat(value);

        if (value.endsWith("rad"))
            return Number.parseFloat(value) * 180 / Math.PI;

        if (value.endsWith("grad"))
            return Number.parseFloat(value) * 0.9;

        if (value.endsWith("turn"))
            return Number.parseFloat(value) * 360;

        return Number.parseFloat(value);
    }

    #parseLength(value: string) : number | null {
        if (!value) return null
        const rootFontSize = 16.0;
        const parentFontSize = 16.0;
        const exSize = 8.0;
        const chSize = 8.0;
    
        if (value.endsWith("px")) return Number.parseFloat(value);
        if (value.endsWith("pt")) return Number.parseFloat(value) * 1.3333333730697632;
        if (value.endsWith("pc")) return Number.parseFloat(value) * 16.0;
        if (value.endsWith("in")) return Number.parseFloat(value) * 96.0;
        if (value.endsWith("cm")) return Number.parseFloat(value) * 37.79527559055118;
        if (value.endsWith("mm")) return Number.parseFloat(value) * 3.7795275590551185;
        if (value.endsWith("q"))  return Number.parseFloat(value) * 0.9448818897637796;
        if (value.endsWith("em")) return Number.parseFloat(value) * parentFontSize;
        if (value.endsWith("rem")) return Number.parseFloat(value) * rootFontSize;
        if (value.endsWith("ex")) return Number.parseFloat(value) * exSize;
        if (value.endsWith("ch")) return Number.parseFloat(value) * chSize;
        if (value.endsWith("%"))  return Number.parseFloat(value) * parentFontSize / 100.0;
        if (value.endsWith("px")){
            return Number.parseFloat(value);
        } else if (value.endsWith("pt")){
            return Number.parseFloat(value)*1.3333333730697632;
        }
        return null
    }

    setMatrixValue(value: string) {
        value = value.trim();

        if (value.length === 0)
            throw new DOMException("Invalid transform.", "SyntaxError");

        // this.identity();

        const transforms = this.#parseTransformList(value);

        if (transforms == "error") {
            throwParseErrorDMatrix("setMatrixValue",value)
            return;
        }

        for (const transform of transforms) {

            const args = this.#splitArguments(transform.args);

            switch (transform.name) {
                case "matrix": {
                    if (args.length !== 6)
                        throw new DOMException("matrix() requires 6 arguments.", "SyntaxError");

                    let a = Number.parseFloat(args[0])
                    let b = Number.parseFloat(args[1])
                    let c = Number.parseFloat(args[2])
                    let d = Number.parseFloat(args[3])

                    if (isNaN(a) || isNaN(b) || isNaN(c) || isNaN(d)) throwParseErrorDMatrix("setMatrixValue",value)

                    this.a = a;
                    this.b = b;
                    this.c = c;
                    this.d = d;
                    this.e = this.#parseLength(args[4]);
                    this.f = this.#parseLength(args[5]);

                    this.is2D = true;
                    break;
                }
                case "matrix3d": {
                    if (args.length !== 16)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    for (let i = 0; i < 16; i++)
                        this.#internal[i] = Number.parseFloat(args[i]);

                    this.is2D = false;
                    break;
                }
                case "translate": {
                    if (args.length < 1 || args.length > 2)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    let lx = this.#parseLength(args[0])
                    let ly = this.#parseLength(args[1])
                    if (!lx){
                        throwParseErrorDMatrix("setMatrixValue",value)
                        return;
                    }

                    const tx = lx;
                    const ty = args.length === 2 ? ly??0 : 0;
                    
                    this.translateSelf(tx, ty);
                    break;
                }

                case "translateX": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    let l = this.#parseLength(args[0])
                    if (!l){
                        throwParseErrorDMatrix("setMatrixValue",value)
                        return;
                    }
                    this.translateSelf(l, 0);
                    break;
                }

                case "translateY": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    let l = this.#parseLength(args[0])
                    if (!l){
                        throwParseErrorDMatrix("setMatrixValue",value)
                        return;
                    }
                    this.translateSelf(0, l);
                    break;
                }

                case "translateZ": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    let l = this.#parseLength(args[0])
                    if (!l){
                        throwParseErrorDMatrix("setMatrixValue",value)
                        return;
                    }
                    this.translateSelf(0, 0, l);
                    break;
                }

                case "translate3d": {
                    if (args.length !== 3)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    let la = this.#parseLength(args[0])
                    let lb = this.#parseLength(args[0])
                    let lc = this.#parseLength(args[0])
                    if (!la || !lb || !lc){
                        throwParseErrorDMatrix("setMatrixValue",value)
                        return;
                    }

                    this.translateSelf(
                        la,
                        lb,
                        lc
                    );

                    break;
                }

                case "scale": {
                    if (args.length < 1 || args.length > 2)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    const sx = Number.parseFloat(args[0]);
                    const sy = args.length === 2 ? Number.parseFloat(args[1]) : sx;

                    this.scaleSelf(sx, sy);
                    break;
                }

                case "scaleX": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.scaleSelf(Number.parseFloat(args[0]), 1);
                    break;
                }

                case "scaleY": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.scaleSelf(1, Number.parseFloat(args[0]));
                    break;
                }

                case "scaleZ": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.scaleSelf(1, 1, Number.parseFloat(args[0]));
                    break;
                }

                case "scale3d": {
                    if (args.length !== 3)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.scale3dSelf(
                        Number.parseFloat(args[0]),
                        Number.parseFloat(args[1]),
                        Number.parseFloat(args[2])
                    );

                    break;
                }

                case "rotate": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.rotateSelf(this.#parseAngle(args[0]));
                    break;
                }

                case "rotateX": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.rotateSelf(this.#parseAngle(args[0]), 0, 0);
                    break;
                }

                case "rotateY": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.rotateSelf(0, this.#parseAngle(args[0]), 0);
                    break;
                }

                case "rotateZ": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.rotateSelf(0, 0, this.#parseAngle(args[0]));
                    break;
                }

                case "rotate3d": {
                    if (args.length !== 4)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.rotateAxisAngleSelf(
                        Number.parseFloat(args[0]),
                        Number.parseFloat(args[1]),
                        Number.parseFloat(args[2]),
                        this.#parseAngle(args[3])
                    );

                    break;
                }

                case "skew": {
                    if (args.length < 1 || args.length > 2)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.skewXSelf(this.#parseAngle(args[0]));

                    if (args.length === 2)
                        this.skewYSelf(this.#parseAngle(args[1]));

                    break;
                }

                case "skewX": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.skewXSelf(this.#parseAngle(args[0]));
                    break;
                }

                case "skewY": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    this.skewYSelf(this.#parseAngle(args[0]));
                    break;
                }

                case "perspective": {
                    if (args.length !== 1)
                        throwParseErrorDMatrix("setMatrixValue",value)

                    const d = this.#parseLength(args[0]);

                    if (d === 0 || !d){
                        throwParseErrorDMatrix("setMatrixValue",value)
                        return;
                    }

                    this.#internal.set(multipliedMatrices(
                        this.#internal,
                        new Float64Array([
                            1, 0, 0,      0,
                            0, 1, 0,      0,
                            0, 0, 1, -1 / d,
                            0, 0, 0,      1
                        ])
                    ));

                    this.is2D = false;
                    break;
                }

                default:
                    throwParseErrorDMatrix("setMatrixValue",value)
            }
        }

        return this;
    }

    toString(){
        return this.is2D?
        //If 2d
        `matrix(${this.a}, ${this.b}, ${this.c}, ${this.d}, ${this.e}, ${this.f})`
        :
        `matrix3d(${this.m11}, ${this.m12}, ${this.m13}, ${this.m14}, ${this.m21}, ${this.m22}, ${this.m23}, ${this.m24}, ${this.m31}, ${this.m32}, ${this.m33}, ${this.m34}, ${this.m41}, ${this.m42}, ${this.m43}, ${this.m44})`
    }

    constructor();
    constructor(matrix : Array<number> | ArrayBufferLike | ArrayLike<number>);
    constructor(...args : any){
        if (!isMatrixArray(args[0])) throw new SyntaxError(`Failed to construct 'DOMMatrix': Failed to parse '${args[0]}'.`);
        //If no args are passed (default constructor), then 
        if (args[0]) {
            const len = args[0].length;
            if (len != 6 && len != 16) throw new TypeError(`Failed to construct 'DOMMatrix': The sequence must contain 6 elements for a 2D matrix or 16 elements for a 3D matrix.`) 
            let matrix = args[0];
            if (len == 6) {
                this.a = matrix[0]
                this.b = matrix[1]
                this.c = matrix[2]
                this.d = matrix[3]
                this.e = matrix[4]
                this.f = matrix[5]
            }else {
                let index = 0;
                for (const item of matrix){
                    this.#internal[index] = item;
                    index++;
                }
            }
            this.#isIdentity = matrix[0]  == 1 && matrix[1]  == 0 && matrix[2]  == 0 && matrix[3]  == 0 &&
            matrix[4]  == 0 && matrix[5]  == 1 && matrix[6]  == 0 && matrix[7]  == 0 &&
            matrix[8]  == 0 && matrix[9]  == 0 && matrix[10] == 1 && matrix[11] == 0 &&
            matrix[12] == 0 && matrix[13] == 0 && matrix[14] == 0 && matrix[15] == 1;
            // this.a = this.m11
            // this.b = this.m21
            // this.c = this.m12
            // this.d = this.m22
            // this.e = this.m41
            // this.f = this.m42
        }
    }

    get ptr(){
        return ptr(this.#internal)
    }

    static fromFloat32Array(arr : Float32Array) : DOMMatrix | void {
        if (arr instanceof Float32Array) return new DOMMatrix(arr);
        throw new TypeError(`Failed to execute 'fromFloat32Array' on 'DOMMatrix': parameter 1 is not of type 'Float32Array'.`)
    }
    static fromFloat64Array(arr : Float64Array) : DOMMatrix | void {
        if (arr instanceof Float64Array) return new DOMMatrix(arr);
        throw new TypeError(`Failed to execute 'fromFloat64Array' on 'DOMMatrix': parameter 1 is not of type 'Float64Array'.`)
    }
    static fromMatrix(arr : any) : DOMMatrix {
        if (!arr) return new DOMMatrix();
        if (
            arr.a != arr.m11 ||
            arr.b != arr.m21 ||
            arr.c != arr.m12 ||
            arr.d != arr.m22 ||
            arr.e != arr.m41 ||
            arr.f != arr.m42
        ) throw new TypeError(`Failed to execute 'fromMatrix' on 'DOMMatrix': Property mismatch on matrix initialization.`)
        let a = arr.a ?? undefined;
        let b = arr.b ?? undefined;
        let c = arr.c ?? undefined;
        let d = arr.d ?? undefined;
        let e = arr.e ?? undefined;
        let f = arr.f ?? undefined;
        let m11 = arr.m11 ?? a ?? 1;
        let m12 = arr.m12 ?? b ?? 0;
        let m13 = arr.m13 ?? 0;
        let m14 = arr.m14 ?? 0;
        let m21 = arr.m21 ?? c ?? 0;
        let m22 = arr.m22 ?? d ?? 1;
        let m23 = arr.m23 ?? 0;
        let m24 = arr.m24 ?? 0;
        let m31 = arr.m31 ?? 0;
        let m32 = arr.m32 ?? 0;
        let m33 = arr.m33 ?? 1;
        let m34 = arr.m34 ?? 0;
        let m41 = arr.m41 ?? e ?? 0;
        let m42 = arr.m42 ?? f ?? 0;
        let m43 = arr.m43 ?? 0;
        let m44 = arr.m44 ?? 1;

        return new DOMMatrix([m11,m12,m13,m14,m21,m22,m23,m24,m31,m32,m33,m34,m41,m42,m43,m44])
    }
}

{
	const proto = DOMMatrix.prototype;
	
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