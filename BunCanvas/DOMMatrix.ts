export class DOMMatrix {
    a : number;
    b : number;
    c : number;
    d : number;
    e : number;
    f : number;
    is2D : boolean;
    isIdentity : boolean;
    m11 : number;
    m12 : number;
    m13 : number;
    m14 : number;
    m21 : number;
    m22 : number;
    m23 : number;
    m24 : number;
    m31 : number;
    m32 : number;
    m33 : number;
    m34 : number;
    m41 : number;
    m42 : number;
    m43 : number;
    m44 : number;

    constructor(matrix : Float32Array, is2D : boolean, isIdentity : boolean){ 
        [this.m11,this.m12,this.m13,this.m14,this.m21,this.m22,this.m23,this.m24,this.m31,this.m32,this.m33,this.m34,this.m41,this.m42,this.m43,this.m44] = matrix
        this.a = this.m11
        this.b = this.m21
        this.c = this.m12
        this.d = this.m22
        this.e = this.m41
        this.f = this.m42
        this.is2D = is2D
        this.isIdentity = isIdentity
    }
}