import { ptr } from "bun:ffi";

export interface DOMMatrix2DInit {
    a?: number;
    b?: number;
    c?: number;
    d?: number;
    e?: number;
    f?: number;
    m11?: number;
    m12?: number;
    m21?: number;
    m22?: number;
    m41?: number;
    m42?: number;
}

export interface DOMMatrixInit extends DOMMatrix2DInit {
    m13?: number;
    m14?: number;
    m23?: number;
    m24?: number;
    m31?: number;
    m32?: number;
    m33?: number;
    m34?: number;
    m43?: number;
    m44?: number;
    is2D?: boolean;
}

export interface DOMPointInit {
    x?: number;
    y?: number;
    z?: number;
    w?: number;
}

function isMatrixArray(
    value: unknown
): value is number[] | Float32Array | Float64Array {
    return (
        Array.isArray(value) ||
        (ArrayBuffer.isView(value) && !(value instanceof DataView))
    );
}

/**
 * Column-major 4x4 matrix multiplication: returns a * b
 * Index mapping (column-major):
 *  0: m11,  4: m21,  8: m31, 12: m41
 *  1: m12,  5: m22,  9: m32, 13: m42
 *  2: m13,  6: m23, 10: m33, 14: m43
 *  3: m14,  7: m24, 11: m34, 15: m44
 */
export function multipliedMatrices(
    a: Float64Array | Float32Array,
    b: Float64Array | Float32Array
): Float64Array | number[] {
    const out = [];

    const a00 = a[0], a01 = a[4], a02 = a[8],  a03 = a[12];
    const a10 = a[1], a11 = a[5], a12 = a[9],  a13 = a[13];
    const a20 = a[2], a21 = a[6], a22 = a[10], a23 = a[14];
    const a30 = a[3], a31 = a[7], a32 = a[11], a33 = a[15];

    const b00 = b[0], b01 = b[4], b02 = b[8],  b03 = b[12];
    const b10 = b[1], b11 = b[5], b12 = b[9],  b13 = b[13];
    const b20 = b[2], b21 = b[6], b22 = b[10], b23 = b[14];
    const b30 = b[3], b31 = b[7], b32 = b[11], b33 = b[15];

    out[0]  = a00 * b00 + a01 * b10 + a02 * b20 + a03 * b30;
    out[1]  = a10 * b00 + a11 * b10 + a12 * b20 + a13 * b30;
    out[2]  = a20 * b00 + a21 * b10 + a22 * b20 + a23 * b30;
    out[3]  = a30 * b00 + a31 * b10 + a32 * b20 + a33 * b30;

    out[4]  = a00 * b01 + a01 * b11 + a02 * b21 + a03 * b31;
    out[5]  = a10 * b01 + a11 * b11 + a12 * b21 + a13 * b31;
    out[6]  = a20 * b01 + a21 * b11 + a22 * b21 + a23 * b31;
    out[7]  = a30 * b01 + a31 * b11 + a32 * b21 + a33 * b31;

    out[8]  = a00 * b02 + a01 * b12 + a02 * b22 + a03 * b32;
    out[9]  = a10 * b02 + a11 * b12 + a12 * b22 + a13 * b32;
    out[10] = a20 * b02 + a21 * b12 + a22 * b22 + a23 * b32;
    out[11] = a30 * b02 + a31 * b12 + a32 * b22 + a33 * b32;

    out[12] = a00 * b03 + a01 * b13 + a02 * b23 + a03 * b33;
    out[13] = a10 * b03 + a11 * b13 + a12 * b23 + a13 * b33;
    out[14] = a20 * b03 + a21 * b13 + a22 * b23 + a23 * b33;
    out[15] = a30 * b03 + a31 * b13 + a32 * b23 + a33 * b33;

    return out;
}

function throwParseErrorDMatrix(fName: string, pString: string): never {
    throw new SyntaxError(
        `Failed to execute '${fName}' on 'DOMMatrix': Failed to parse '${pString}'.`
    );
}

const CSS_NUMBER_RE = /^[+-]?(?:\d+\.?\d*|\.\d+)(?:e[+-]?\d+)?$/i;
const CSS_DIM_RE = /^([+-]?(?:\d+\.?\d*|\.\d+)(?:e[+-]?\d+)?)([a-z%]+)$/i;

const f64Arrays = new WeakMap<DOMMatrix,Float64Array>();

export function getInternalArray(dMtrx : DOMMatrix) : Float64Array | undefined {
    return f64Arrays.get(dMtrx)
}

export class DOMMatrix {
    // Column-major 4x4 identity matrix
    #internal = new Float64Array([
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    ]);
    #is2D = true;

    get is2D(): boolean {
        return this.#is2D;
    }

    get isIdentity(): boolean {
        const m = this.#internal;
        return (
            m[0] === 1 && m[1] === 0 && m[2] === 0 && m[3] === 0 &&
            m[4] === 0 && m[5] === 1 && m[6] === 0 && m[7] === 0 &&
            m[8] === 0 && m[9] === 0 && m[10] === 1 && m[11] === 0 &&
            m[12] === 0 && m[13] === 0 && m[14] === 0 && m[15] === 1
        );
    }

    // 2D Aliases
    get a(): number { return this.#internal[0]; }
    set a(v: number) { this.#internal[0] = Number(v); }

    get b(): number { return this.#internal[1]; }
    set b(v: number) { this.#internal[1] = Number(v); }

    get c(): number { return this.#internal[4]; }
    set c(v: number) { this.#internal[4] = Number(v); }

    get d(): number { return this.#internal[5]; }
    set d(v: number) { this.#internal[5] = Number(v); }

    get e(): number { return this.#internal[12]; }
    set e(v: number) { this.#internal[12] = Number(v); }

    get f(): number { return this.#internal[13]; }
    set f(v: number) { this.#internal[13] = Number(v); }

    // Column 1
    get m11(): number { return this.#internal[0]; }
    set m11(v: number) { this.#internal[0] = Number(v); }

    get m12(): number { return this.#internal[1]; }
    set m12(v: number) { this.#internal[1] = Number(v); }

    get m13(): number { return this.#internal[2]; }
    set m13(v: number) {
        const val = Number(v);
        this.#internal[2] = val;
        if (val !== 0) this.#is2D = false;
    }

    get m14(): number { return this.#internal[3]; }
    set m14(v: number) {
        const val = Number(v);
        this.#internal[3] = val;
        if (val !== 0) this.#is2D = false;
    }

    // Column 2
    get m21(): number { return this.#internal[4]; }
    set m21(v: number) { this.#internal[4] = Number(v); }

    get m22(): number { return this.#internal[5]; }
    set m22(v: number) { this.#internal[5] = Number(v); }

    get m23(): number { return this.#internal[6]; }
    set m23(v: number) {
        const val = Number(v);
        this.#internal[6] = val;
        if (val !== 0) this.#is2D = false;
    }

    get m24(): number { return this.#internal[7]; }
    set m24(v: number) {
        const val = Number(v);
        this.#internal[7] = val;
        if (val !== 0) this.#is2D = false;
    }

    // Column 3
    get m31(): number { return this.#internal[8]; }
    set m31(v: number) {
        const val = Number(v);
        this.#internal[8] = val;
        if (val !== 0) this.#is2D = false;
    }

    get m32(): number { return this.#internal[9]; }
    set m32(v: number) {
        const val = Number(v);
        this.#internal[9] = val;
        if (val !== 0) this.#is2D = false;
    }

    get m33(): number { return this.#internal[10]; }
    set m33(v: number) {
        const val = Number(v);
        this.#internal[10] = val;
        if (val !== 1) this.#is2D = false;
    }

    get m34(): number { return this.#internal[11]; }
    set m34(v: number) {
        const val = Number(v);
        this.#internal[11] = val;
        if (val !== 0) this.#is2D = false;
    }

    // Column 4
    get m41(): number { return this.#internal[12]; }
    set m41(v: number) { this.#internal[12] = Number(v); }

    get m42(): number { return this.#internal[13]; }
    set m42(v: number) { this.#internal[13] = Number(v); }

    get m43(): number { return this.#internal[14]; }
    set m43(v: number) {
        const val = Number(v);
        this.#internal[14] = val;
        if (val !== 0) this.#is2D = false;
    }

    get m44(): number { return this.#internal[15]; }
    set m44(v: number) {
        const val = Number(v);
        this.#internal[15] = val;
        if (val !== 1) this.#is2D = false;
    }

    constructor();
    constructor(init: string | number[] | Float32Array | Float64Array);
    constructor(init?: string | number[] | Float32Array | Float64Array) {
        f64Arrays.set(this,this.#internal)
        if (init === undefined) {
            return;
        }

        if (typeof init === "string") {
            this.#parseAndApplyTransformList(init, "constructor");
            return;
        }

        if (!isMatrixArray(init)) {
            throw new SyntaxError(
                `Failed to construct 'DOMMatrix': Failed to parse '${init}'.`
            );
        }

        const len = init.length;
        if (len !== 6 && len !== 16) {
            throw new TypeError(
                `Failed to construct 'DOMMatrix': The sequence must contain 6 elements for a 2D matrix or 16 elements for a 3D matrix.`
            );
        }

        if (len === 6) {
            this.#internal[0] = init[0];
            this.#internal[1] = init[1];
            this.#internal[4] = init[2];
            this.#internal[5] = init[3];
            this.#internal[12] = init[4];
            this.#internal[13] = init[5];
            this.#is2D = true;
        } else {
            for (let i = 0; i < 16; i++) {
                this.#internal[i] = init[i];
            }
            this.#is2D = false;
        }
    }

    #clone(): DOMMatrix {
        const copy = new DOMMatrix();
        copy.#internal.set(this.#internal);
        copy.#is2D = this.#is2D;
        return copy;
    }

    // Mutable Transformations

    multiplySelf(other?: DOMMatrixInit): this {
        const otherMatrix =
            other instanceof DOMMatrix ? other : DOMMatrix.fromMatrix(other);
        this.#internal.set(
            multipliedMatrices(this.#internal, otherMatrix.#internal)
        );
        if (!otherMatrix.is2D) {
            this.#is2D = false;
        }
        return this;
    }

    preMultiplySelf(other?: DOMMatrixInit): this {
        const otherMatrix =
            other instanceof DOMMatrix ? other : DOMMatrix.fromMatrix(other);
        this.#internal.set(
            multipliedMatrices(otherMatrix.#internal, this.#internal)
        );
        if (!otherMatrix.is2D) {
            this.#is2D = false;
        }
        return this;
    }

    translateSelf(tx = 0, ty = 0, tz = 0): this {
        this.#internal[12]+=tx
        this.#internal[13]+=ty
        this.#internal[14]+=tz
        if (tz !== 0) {
            this.#is2D = false;
        }
        return this;
    }

    scaleSelf(
        scaleX = 1,
        scaleY = scaleX,
        scaleZ = 1,
        originX = 0,
        originY = 0,
        originZ = 0
    ): this {
        if (originX !== 0 || originY !== 0 || originZ !== 0) {
            this.translateSelf(originX, originY, originZ);
        }
        this.#internal[0] += scaleX;
        this.#internal[5] += scaleY;
        this.#internal[10] += scaleZ;

        if (originX !== 0 || originY !== 0 || originZ !== 0) {
            this.translateSelf(-originX, -originY, -originZ);
        }

        if (scaleZ !== 1 || originZ !== 0) {
            this.#is2D = false;
        }
        return this;
    }

    scale3dSelf(scale = 1, originX = 0, originY = 0, originZ = 0): this {
        return this.scaleSelf(scale, scale, scale, originX, originY, originZ);
    }

    rotateSelf(rotX = 0, rotY?: number, rotZ?: number): this {
        if (rotY === undefined && rotZ === undefined) {
            rotZ = rotX;
            rotX = 0;
            rotY = 0;
        } else {
            rotY ??= 0;
            rotZ ??= 0;
        }

        const deg2rad = Math.PI / 180;

        // Spec order: post-multiply Rz, then post-multiply Ry, then post-multiply Rx
        if (rotZ !== 0) {
            const radZ = rotZ * deg2rad;
            const sz = Math.sin(radZ);
            const cz = Math.cos(radZ);
            // this.#internal.set(
            //     multipliedMatrices(
            //         this.#internal,
            //         new Float64Array([
            //             cz, sz, 0, 0,
            //             -sz, cz, 0, 0,
            //             0, 0, 1, 0,
            //             0, 0, 0, 1
            //         ])
            //     )
            // );
            this.#internal[0] += cz;
            this.#internal[1] += sz;
            this.#internal[4] -= sz;
            this.#internal[5] += cz;
        }

        if (rotY !== 0) {
            const radY = rotY * deg2rad;
            const sy = Math.sin(radY);
            const cy = Math.cos(radY);
            // this.#internal.set(
            //     multipliedMatrices(
            //         this.#internal,
            //         new Float64Array([
            //             cy, 0, -sy, 0,
            //             0, 1, 0, 0,
            //             sy, 0, cy, 0,
            //             0, 0, 0, 1
            //         ])
            //     )
            // );
            this.#internal[0] += cy;
            this.#internal[2] -= sy;
            this.#internal[8] += sy;
            this.#internal[10] += cy;
        }

        if (rotX !== 0) {
            const radX = rotX * deg2rad;
            const sx = Math.sin(radX);
            const cx = Math.cos(radX);
            // this.#internal.set(
            //     multipliedMatrices(
            //         this.#internal,
            //         new Float64Array([
            //             1, 0, 0, 0,
            //             0, cx, sx, 0,
            //             0, -sx, cx, 0,
            //             0, 0, 0, 1
            //         ])
            //     )
            // );
            this.#internal[5] += cx;
            this.#internal[6] += sx;
            this.#internal[9] -= sx;
            this.#internal[10] += cx;
        }

        if (rotX !== 0 || rotY !== 0) {
            this.#is2D = false;
        }

        return this;
    }

    rotateAxisAngleSelf(x = 0, y = 0, z = 1, angle = 0): this {
        const length = Math.hypot(x, y, z);
        if (length === 0) {
            return this;
        }

        x /= length;
        y /= length;
        z /= length;

        const rad = (angle * Math.PI) / 180;
        const s = Math.sin(rad);
        const c = Math.cos(rad);
        const t = 1 - c;

        // Column-major Rodrigues' rotation matrix
        // const r = new Float64Array([
        //     t * x * x + c,     t * x * y + s * z, t * x * z - s * y, 0,
        //     t * x * y - s * z, t * y * y + c,     t * y * z + s * x, 0,
        //     t * x * z + s * y, t * y * z - s * x, t * z * z + c,     0,
        //     0,                 0,                 0,                 1
        // ]);

        // this.#internal.set(multipliedMatrices(this.#internal, r));

        this.#internal[0] += t * x * x + c
        this.#internal[1] += t * x * y + s * z
        this.#internal[2] += t * x * z - s * y

        this.#internal[4] += t * x * y - s * z
        this.#internal[5] += t * y * y + c
        this.#internal[6] += t * y * z + s * x

        this.#internal[8] += t * x * z + s * y
        this.#internal[9] += t * y * z - s * x
        this.#internal[10] += t * z * z + c

        if (x !== 0 || y !== 0) {
            this.#is2D = false;
        }

        return this;
    }

    rotateFromVectorSelf(x = 0, y = 0): this {
        if (x === 0 && y === 0) {
            return this;
        }
        const angle = (Math.atan2(y, x) * 180) / Math.PI;
        return this.rotateSelf(0, 0, angle);
    }

    skewXSelf(sx = 0): this {
        const t = Math.tan((sx * Math.PI) / 180);
        // this.#internal.set(
        //     multipliedMatrices(
        //         this.#internal,
        //         new Float64Array([
        //             1, 0, 0, 0,
        //             t, 1, 0, 0,
        //             0, 0, 1, 0,
        //             0, 0, 0, 1
        //         ])
        //     )
        // );
        this.#internal[4] += t;
        return this;
    }

    skewYSelf(sy = 0): this {
        const t = Math.tan((sy * Math.PI) / 180);
        // this.#internal.set(
        //     multipliedMatrices(
        //         this.#internal,
        //         new Float64Array([
        //             1, t, 0, 0,
        //             0, 1, 0, 0,
        //             0, 0, 1, 0,
        //             0, 0, 0, 1
        //         ])
        //     )
        // );
        this.#internal[1] += t
        return this;
    }

    invertSelf(): this {
        const m = this.#internal;
        const m00 = m[0],  m01 = m[4],  m02 = m[8],  m03 = m[12];
        const m10 = m[1],  m11 = m[5],  m12 = m[9],  m13 = m[13];
        const m20 = m[2],  m21 = m[6],  m22 = m[10], m23 = m[14];
        const m30 = m[3],  m31 = m[7],  m32 = m[11], m33 = m[15];

        const b00 = m00 * m11 - m10 * m01;
        const b01 = m00 * m21 - m20 * m01;
        const b02 = m00 * m31 - m30 * m01;
        const b03 = m10 * m21 - m20 * m11;
        const b04 = m10 * m31 - m30 * m11;
        const b05 = m20 * m31 - m30 * m21;
        const b06 = m02 * m13 - m12 * m03;
        const b07 = m02 * m23 - m22 * m03;
        const b08 = m02 * m33 - m32 * m03;
        const b09 = m12 * m23 - m22 * m13;
        const b10 = m12 * m33 - m32 * m13;
        const b11 = m22 * m33 - m32 * m23;

        const det =
            b00 * b11 - b01 * b10 + b02 * b09 +
            b03 * b08 - b04 * b07 + b05 * b06;

        if (!det || !Number.isFinite(det)) {
            this.#internal.fill(NaN);
            this.#is2D = false;
            return this;
        }

        const invDet = 1.0 / det;

        m[0]  = (m11 * b11 - m21 * b10 + m31 * b09) * invDet;
        m[1]  = (m20 * b10 - m10 * b11 - m30 * b09) * invDet;
        m[2]  = (m13 * b05 - m23 * b04 + m33 * b03) * invDet;
        m[3]  = (m22 * b04 - m12 * b05 - m32 * b03) * invDet;

        m[4]  = (m21 * b08 - m01 * b11 - m31 * b07) * invDet;
        m[5]  = (m00 * b11 - m20 * b08 + m30 * b07) * invDet;
        m[6]  = (m23 * b02 - m03 * b05 - m33 * b01) * invDet;
        m[7]  = (m02 * b05 - m22 * b02 + m32 * b01) * invDet;

        m[8]  = (m01 * b10 - m11 * b08 + m31 * b06) * invDet;
        m[9]  = (m10 * b08 - m00 * b10 - m30 * b06) * invDet;
        m[10] = (m03 * b04 - m13 * b02 + m33 * b00) * invDet;
        m[11] = (m12 * b02 - m02 * b04 - m32 * b00) * invDet;

        m[12] = (m11 * b07 - m01 * b09 - m21 * b06) * invDet;
        m[13] = (m00 * b09 - m10 * b07 + m20 * b06) * invDet;
        m[14] = (m13 * b01 - m03 * b03 - m23 * b00) * invDet;
        m[15] = (m02 * b03 - m12 * b01 + m22 * b00) * invDet;

        return this;
    }

    // --- Immutable Transformations ---

    translate(tx = 0, ty = 0, tz = 0): DOMMatrix {
        return this.#clone().translateSelf(tx, ty, tz);
    }

    scale(
        scaleX = 1,
        scaleY = scaleX,
        scaleZ = 1,
        originX = 0,
        originY = 0,
        originZ = 0
    ): DOMMatrix {
        return this.#clone().scaleSelf(
            scaleX,
            scaleY,
            scaleZ,
            originX,
            originY,
            originZ
        );
    }

    scale3d(scale = 1, originX = 0, originY = 0, originZ = 0): DOMMatrix {
        return this.#clone().scale3dSelf(scale, originX, originY, originZ);
    }

    scaleNonUniform(scaleX = 1, scaleY = 1): DOMMatrix {
        return this.#clone().scaleSelf(scaleX, scaleY, 1, 0, 0, 0);
    }

    rotate(rotX = 0, rotY?: number, rotZ?: number): DOMMatrix {
        return this.#clone().rotateSelf(rotX, rotY, rotZ);
    }

    rotateAxisAngle(x = 0, y = 0, z = 1, angle = 0): DOMMatrix {
        return this.#clone().rotateAxisAngleSelf(x, y, z, angle);
    }

    rotateFromVector(x = 0, y = 0): DOMMatrix {
        return this.#clone().rotateFromVectorSelf(x, y);
    }

    skewX(sx = 0): DOMMatrix {
        return this.#clone().skewXSelf(sx);
    }

    skewY(sy = 0): DOMMatrix {
        return this.#clone().skewYSelf(sy);
    }

    multiply(other?: DOMMatrixInit): DOMMatrix {
        return this.#clone().multiplySelf(other);
    }

    flipX(): DOMMatrix {
        const copy = this.#clone();
        // copy.#internal.set(
        //     multipliedMatrices(
        //         copy.#internal,
        //         new Float64Array([
        //             -1, 0, 0, 0,
        //             0, 1, 0, 0,
        //             0, 0, 1, 0,
        //             0, 0, 0, 1
        //         ])
        //     )
        // );
        copy.#internal[0]*=-1
        return copy;
    }

    flipY(): DOMMatrix {
        const copy = this.#clone();
        // copy.#internal.set(
        //     multipliedMatrices(
        //         copy.#internal,
        //         new Float64Array([
        //             1, 0, 0, 0,
        //             0, -1, 0, 0,
        //             0, 0, 1, 0,
        //             0, 0, 0, 1
        //         ])
        //     )
        // );
        copy.#internal[5]*=-1
        return copy;
    }

    inverse(): DOMMatrix {
        return this.#clone().invertSelf();
    }

    transformPoint(point?: DOMPointInit): { x: number; y: number; z: number; w: number } {
        const x = point?.x ?? 0;
        const y = point?.y ?? 0;
        const z = point?.z ?? 0;
        const w = point?.w ?? 1;
        const m = this.#internal;

        return {
            x: m[0] * x + m[4] * y + m[8] * z + m[12] * w,
            y: m[1] * x + m[5] * y + m[9] * z + m[13] * w,
            z: m[2] * x + m[6] * y + m[10] * z + m[14] * w,
            w: m[3] * x + m[7] * y + m[11] * z + m[15] * w
        };
    }

    toFloat32Array(): Float32Array {
        return new Float32Array(this.#internal);
    }

    toFloat64Array(): Float64Array {
        return new Float64Array(this.#internal);
    }

    toJSON() {
        return {
            a: this.a,
            b: this.b,
            c: this.c,
            d: this.d,
            e: this.e,
            f: this.f,
            m11: this.m11,
            m12: this.m12,
            m13: this.m13,
            m14: this.m14,
            m21: this.m21,
            m22: this.m22,
            m23: this.m23,
            m24: this.m24,
            m31: this.m31,
            m32: this.m32,
            m33: this.m33,
            m34: this.m34,
            m41: this.m41,
            m42: this.m42,
            m43: this.m43,
            m44: this.m44,
            is2D: this.is2D,
            isIdentity: this.isIdentity
        };
    }

    // --- CSS Transform Parsing ---

    #parseTransformList(value: string) {
        const transforms: { name: string; args: string }[] = [];
        let i = 0;

        while (i < value.length) {
            while (/\s/.test(value[i])) i++;
            if (i >= value.length) break;

            const nameStart = i;
            while (/[A-Za-z0-9-]/.test(value[i])) i++;
            const name = value.substring(nameStart, i);

            if (!name || value[i] !== "(") {
                return "error";
            }

            i++; // skip '('
            let depth = 1;
            const argsStart = i;

            while (i < value.length && depth > 0) {
                if (value[i] === "(") depth++;
                else if (value[i] === ")") depth--;
                i++;
            }

            if (depth !== 0) return "error";

            transforms.push({
                name,
                args: value.substring(argsStart, i - 1)
            });
        }

        return transforms;
    }

    #splitArguments(args: string): string[] | null {
        const trimmed = args.trim();
        if (!trimmed) return [];

        // Reject leading, trailing, or consecutive commas
        if (trimmed.startsWith(",") || trimmed.endsWith(",") || /,\s*,/.test(trimmed)) {
            return null;
        }

        return trimmed.split(/\s*,\s*/);
    }

    #parseNumber(value: string): number | null {
        if (!CSS_NUMBER_RE.test(value)) return null;
        const n = Number.parseFloat(value);
        return Number.isFinite(n) ? n : null;
    }

    #parseAngle(value: string): number | null {
        if (CSS_NUMBER_RE.test(value)) {
            const n = Number.parseFloat(value);
            return n === 0 ? 0 : null; // Only unitless 0 is valid for CSS <angle>
        }

        const match = CSS_DIM_RE.exec(value);
        if (!match) return null;

        const num = Number.parseFloat(match[1]);
        if (!Number.isFinite(num)) return null;

        switch (match[2].toLowerCase()) {
            case "deg":  return num;
            case "rad":  return (num * 180) / Math.PI;
            case "grad": return num * 0.9;
            case "turn": return num * 360;
            default:     return null;
        }
    }

    #parseLength(value: string): number | null {
        if (!value) return null;

        if (CSS_NUMBER_RE.test(value)) {
            const n = Number.parseFloat(value);
            return n === 0 ? 0 : null; // Only unitless 0 is valid for CSS <length>
        }

        const match = CSS_DIM_RE.exec(value);
        if (!match) return null;

        const num = Number.parseFloat(match[1]);
        if (!Number.isFinite(num)) return null;

        const rootFontSize = 16.0;
        const parentFontSize = 16.0;
        const exSize = 8.0;
        const chSize = 8.0;

        switch (match[2].toLowerCase()) {
            case "px":  return num;
            case "pt":  return num * (96 / 72);
            case "pc":  return num * 16.0;
            case "in":  return num * 96.0;
            case "cm":  return num * (96 / 2.54);
            case "mm":  return num * (96 / 25.4);
            case "q":   return num * (96 / 101.6);
            case "em":  return num * parentFontSize;
            case "rem": return num * rootFontSize;
            case "ex":  return num * exSize;
            case "ch":  return num * chSize;
            default:    return null; // Percentages are not allowed in DOMMatrix syntax
        }
    }

    #throwParseOrConstructError(caller: "setMatrixValue" | "constructor", raw: string): never {
        if (caller === "constructor") {
            throw new SyntaxError(
                `Failed to construct 'DOMMatrix': Failed to parse '${raw}'.`
            );
        }
        throwParseErrorDMatrix("setMatrixValue", raw);
    }

    #parseAndApplyTransformList(
        rawInput: string,
        caller: "setMatrixValue" | "constructor"
    ): this {
        const value = rawInput.trim();

        // Spec: empty string or "none" resets to 2D identity matrix
        if (value === "" || value.toLowerCase() === "none") {
            this.#internal.set([
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            ]);
            this.#is2D = true;
            return this;
        }

        const transforms = this.#parseTransformList(value);
        if (transforms === "error" || transforms.length === 0) {
            this.#throwParseOrConstructError(caller, rawInput);
        }

        // Work on a temporary matrix so partial transforms don't mutate `this` on syntax errors
        const temp = new DOMMatrix();

        for (const transform of transforms) {
            const args = this.#splitArguments(transform.args);
            if (!args) {
                this.#throwParseOrConstructError(caller, rawInput);
            }

            switch (transform.name.toLowerCase()) {
                case "matrix": {
                    if (args.length !== 6) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const nums = args.map((arg) => this.#parseNumber(arg));
                    if (nums.some((n) => n === null)) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const [a, b, c, d, e, f] = nums as number[];
                    temp.multiplySelf(new DOMMatrix([a, b, c, d, e, f]));
                    break;
                }

                case "matrix3d": {
                    if (args.length !== 16) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const nums = args.map((arg) => this.#parseNumber(arg));
                    if (nums.some((n) => n === null)) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.multiplySelf(new DOMMatrix(nums as number[]));
                    break;
                }

                case "translate": {
                    if (args.length < 1 || args.length > 2) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const tx = this.#parseLength(args[0]);
                    const ty = args.length === 2 ? this.#parseLength(args[1]) : 0;
                    if (tx === null || ty === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.translateSelf(tx, ty, 0);
                    break;
                }

                case "translatex": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const tx = this.#parseLength(args[0]);
                    if (tx === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.translateSelf(tx, 0, 0);
                    break;
                }

                case "translatey": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const ty = this.#parseLength(args[0]);
                    if (ty === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.translateSelf(0, ty, 0);
                    break;
                }

                case "translatez": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const tz = this.#parseLength(args[0]);
                    if (tz === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.translateSelf(0, 0, tz);
                    break;
                }

                case "translate3d": {
                    if (args.length !== 3) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const tx = this.#parseLength(args[0]);
                    const ty = this.#parseLength(args[1]);
                    const tz = this.#parseLength(args[2]);
                    if (tx === null || ty === null || tz === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.translateSelf(tx, ty, tz);
                    break;
                }

                case "scale": {
                    if (args.length < 1 || args.length > 2) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const sx = this.#parseNumber(args[0]);
                    const sy = args.length === 2 ? this.#parseNumber(args[1]) : sx;
                    if (sx === null || sy === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.scaleSelf(sx, sy, 1);
                    break;
                }

                case "scalex": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const sx = this.#parseNumber(args[0]);
                    if (sx === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.scaleSelf(sx, 1, 1);
                    break;
                }

                case "scaley": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const sy = this.#parseNumber(args[0]);
                    if (sy === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.scaleSelf(1, sy, 1);
                    break;
                }

                case "scalez": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const sz = this.#parseNumber(args[0]);
                    if (sz === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.scaleSelf(1, 1, sz);
                    break;
                }

                case "scale3d": {
                    if (args.length !== 3) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const sx = this.#parseNumber(args[0]);
                    const sy = this.#parseNumber(args[1]);
                    const sz = this.#parseNumber(args[2]);
                    if (sx === null || sy === null || sz === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.scaleSelf(sx, sy, sz);
                    break;
                }

                case "rotate": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const angle = this.#parseAngle(args[0]);
                    if (angle === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.rotateSelf(0, 0, angle);
                    break;
                }

                case "rotatex": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const angle = this.#parseAngle(args[0]);
                    if (angle === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.rotateSelf(angle, 0, 0);
                    break;
                }

                case "rotatey": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const angle = this.#parseAngle(args[0]);
                    if (angle === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.rotateSelf(0, angle, 0);
                    break;
                }

                case "rotatez": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const angle = this.#parseAngle(args[0]);
                    if (angle === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.rotateSelf(0, 0, angle);
                    break;
                }

                case "rotate3d": {
                    if (args.length !== 4) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const x = this.#parseNumber(args[0]);
                    const y = this.#parseNumber(args[1]);
                    const z = this.#parseNumber(args[2]);
                    const angle = this.#parseAngle(args[3]);
                    if (x === null || y === null || z === null || angle === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.rotateAxisAngleSelf(x, y, z, angle);
                    break;
                }

                case "skew": {
                    if (args.length < 1 || args.length > 2) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const ax = this.#parseAngle(args[0]);
                    const ay = args.length === 2 ? this.#parseAngle(args[1]) : 0;
                    if (ax === null || ay === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const tx = Math.tan((ax * Math.PI) / 180);
                    const ty = Math.tan((ay * Math.PI) / 180);
                    // temp.#internal.set(
                    //     multipliedMatrices(
                    //         temp.#internal,
                    //         new Float64Array([
                    //             1, ty, 0, 0,
                    //             tx, 1, 0, 0,
                    //             0, 0, 1, 0,
                    //             0, 0, 0, 1
                    //         ])
                    //     )
                    // );
                    temp.#internal[2] += ty;
                    temp.#internal[4] += tx;
                    break;
                }

                case "skewx": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const ax = this.#parseAngle(args[0]);
                    if (ax === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.skewXSelf(ax);
                    break;
                }

                case "skewy": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const ay = this.#parseAngle(args[0]);
                    if (ay === null) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    temp.skewYSelf(ay);
                    break;
                }

                case "perspective": {
                    if (args.length !== 1) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    if (args[0].toLowerCase() === "none") {
                        break;
                    }
                    const d = this.#parseLength(args[0]);
                    // CSS perspective length cannot be negative
                    if (d === null || d < 0) {
                        this.#throwParseOrConstructError(caller, rawInput);
                    }
                    const m34 = d === 0 ? -Infinity : -1 / d;
                    // temp.#internal.set(
                    //     multipliedMatrices(
                    //         temp.#internal,
                    //         new Float64Array([
                    //             1, 0, 0, 0,
                    //             0, 1, 0, 0,
                    //             0, 0, 1, m34,
                    //             0, 0, 0, 1
                    //         ])
                    //     )
                    // );
                    temp.#internal[11] += m34;
                    temp.#is2D = false;
                    break;
                }

                default:
                    this.#throwParseOrConstructError(caller, rawInput);
            }
        }

        this.#internal.set(temp.#internal);
        this.#is2D = temp.#is2D;
        return this;
    }

    setMatrixValue(value: string): this {
        return this.#parseAndApplyTransformList(String(value), "setMatrixValue");
    }

    toString(): string {
        for (let i = 0; i < 16; i++) {
            if (!Number.isFinite(this.#internal[i])) {
                throw new DOMException(
                    "Failed to execute 'toString' on 'DOMMatrix': Matrix contains non-finite values.",
                    "InvalidStateError"
                );
            }
        }

        return this.is2D
            ? `matrix(${this.a}, ${this.b}, ${this.c}, ${this.d}, ${this.e}, ${this.f})`
            : `matrix3d(${this.m11}, ${this.m12}, ${this.m13}, ${this.m14}, ${this.m21}, ${this.m22}, ${this.m23}, ${this.m24}, ${this.m31}, ${this.m32}, ${this.m33}, ${this.m34}, ${this.m41}, ${this.m42}, ${this.m43}, ${this.m44})`;
    }

    static fromFloat32Array(arr: Float32Array): DOMMatrix {
        if (arr instanceof Float32Array) return new DOMMatrix(arr);
        throw new TypeError(
            `Failed to execute 'fromFloat32Array' on 'DOMMatrix': parameter 1 is not of type 'Float32Array'.`
        );
    }

    static fromFloat64Array(arr: Float64Array): DOMMatrix {
        if (arr instanceof Float64Array) return new DOMMatrix(arr);
        throw new TypeError(
            `Failed to execute 'fromFloat64Array' on 'DOMMatrix': parameter 1 is not of type 'Float64Array'.`
        );
    }

    static fromMatrix(other?: DOMMatrixInit): DOMMatrix {
        if (!other) return new DOMMatrix();

        // Spec validation: a/m11, b/m12, c/m21, d/m22, e/m41, f/m42 must match if both are provided
        if (
            (other.a !== undefined && other.m11 !== undefined && other.a !== other.m11) ||
            (other.b !== undefined && other.m12 !== undefined && other.b !== other.m12) ||
            (other.c !== undefined && other.m21 !== undefined && other.c !== other.m21) ||
            (other.d !== undefined && other.m22 !== undefined && other.d !== other.m22) ||
            (other.e !== undefined && other.m41 !== undefined && other.e !== other.m41) ||
            (other.f !== undefined && other.m42 !== undefined && other.f !== other.m42)
        ) {
            throw new TypeError(
                `Failed to execute 'fromMatrix' on 'DOMMatrix': Property mismatch on matrix initialization.`
            );
        }

        const m11 = other.m11 ?? other.a ?? 1;
        const m12 = other.m12 ?? other.b ?? 0;
        const m13 = other.m13 ?? 0;
        const m14 = other.m14 ?? 0;
        const m21 = other.m21 ?? other.c ?? 0;
        const m22 = other.m22 ?? other.d ?? 1;
        const m23 = other.m23 ?? 0;
        const m24 = other.m24 ?? 0;
        const m31 = other.m31 ?? 0;
        const m32 = other.m32 ?? 0;
        const m33 = other.m33 ?? 1;
        const m34 = other.m34 ?? 0;
        const m41 = other.m41 ?? other.e ?? 0;
        const m42 = other.m42 ?? other.f ?? 0;
        const m43 = other.m43 ?? 0;
        const m44 = other.m44 ?? 1;

        const has3DComponents =
            m13 !== 0 || m14 !== 0 ||
            m23 !== 0 || m24 !== 0 ||
            m31 !== 0 || m32 !== 0 || m33 !== 1 || m34 !== 0 ||
            m43 !== 0 || m44 !== 1;

        if (other.is2D === true && has3DComponents) {
            throw new TypeError(
                `Failed to execute 'fromMatrix' on 'DOMMatrix': Property mismatch on matrix initialization.`
            );
        }

        const is2D = other.is2D !== undefined ? other.is2D : !has3DComponents;

        if (is2D) {
            return new DOMMatrix([m11, m12, m21, m22, m41, m42]);
        }

        return new DOMMatrix([
            m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44
        ]);
    }
}

const proto = DOMMatrix.prototype;

for (const key of Object.getOwnPropertyNames(proto)) {
    const desc = Object.getOwnPropertyDescriptor(proto, key);
    if (key === "constructor") {
        if (desc) {
            desc.value.toString = () =>
                `function ${proto.constructor.name}() { [native code] }`;
        }
        continue;
    }
    if (typeof desc?.value === "function") {
        desc.value.toString = () => `function ${key}() { [native code] }`;
    }
}