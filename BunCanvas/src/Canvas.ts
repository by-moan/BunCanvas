import { lib,encoder, encodeCString } from "../internal/symbols.js";
import { ptr, Pointer, JSCallback } from "bun:ffi"
import { Image } from "./Image.js";
import { ImageData } from "./ImageData.js";
import { DOMMatrix, getInternalArray } from "../dom/DOMMatrix.js";
import { TextMetrics } from "./TextMetrics.js";
import { Path2D } from "./Path2D.js";
import { isValidColor } from "../internal/colorCheck.js";
import { INTERNAL_TOKEN, throwIllegalConstructor } from "../internal/ConstructEnforcer.js";
import { CanvasBlendMode, CanvasBlendModeArg } from "../internal/canvas/blendModes.js";
import { CanvasTextAlignment, CanvasTextAlignmentArg } from "../internal/canvas/textAlignment.js";
import { CanvasTextBaseline, CanvasTextBaselineArg } from "../internal/canvas/textBaseline.js";
import { CanvasLineJoin, CanvasLineJoinArg } from "../internal/canvas/lineJoin.js";
import { CanvasSmoothingQuality, CanvasSmoothingQualityArg } from "../internal/canvas/smoothingQuality.js";
import { ptrs } from "../internal/pointers.js";
import { CanvasBatchOperationType } from "../internal/native/enums.js";
import { writePtrU64 } from "../internal/native/memWrite.js";


const canvasFinalizationRegistry = new FinalizationRegistry((ptr: any)=>{
    lib.symbols.canvas_destroy(ptr);
});

enum GradientType {
    Linear,
    Radial,
    Conic
};


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


class Render2DState {
	lineWidth = 0;
	globalAlpha = 1;
	transform = new DOMMatrix()
	imageSmoothingQuality : CanvasSmoothingQualityArg = "low";
	imageSmoothingEnabled : boolean = true;
	fillStyle : string | CanvasGradient = "#000000";
	strokeStyle : string | CanvasGradient = "#000000";
	compositeOperation : CanvasBlendModeArg = "source-over";
	filter : string = "none";
	fontCss : string = "10px sans-serif";
	textBaseline : CanvasTextBaselineArg = "alphabetic";
	textAlign : CanvasTextAlignmentArg = "start";
	lineDash : number[] = []
	lineDashOffset : number = 0;
	lineJoin : CanvasLineJoinArg = "miter";

	constructor(other? : Render2DState){
		if (other instanceof Render2DState) {
			this.copyFrom(other)
		}
	}

	copyFrom(other : Render2DState){
		if (other instanceof Render2DState) {
			this.lineWidth = other.lineWidth
			this.globalAlpha = other.globalAlpha
			getInternalArray(this.transform)!.set(getInternalArray(other.transform)!)
			// this.transform = DOMMatrix.fromFloat64Array();
			this.imageSmoothingQuality = other.imageSmoothingQuality
			this.imageSmoothingEnabled = other.imageSmoothingEnabled
			this.fillStyle = other.fillStyle
			this.strokeStyle = other.strokeStyle
			this.compositeOperation = other.compositeOperation
			this.filter = other.filter
			this.fontCss = other.fontCss
			this.textBaseline = other.textBaseline
			this.textAlign = other.textAlign
			this.lineDash = [...other.lineDash];
			this.lineDashOffset = other.lineDashOffset
			this.lineJoin = other.lineJoin
		}
	}
}

export class CanvasRenderingContext2D {
	//#region Private Fields & State
	static readonly #MAX_BUF_SIZE = 4 * 1024 * 1024; // 4 MB cap

	#rawBuf = new ArrayBuffer(256 * 1024);
	#i32 = new Int32Array(this.#rawBuf);
	#f32 = new Float32Array(this.#rawBuf);
	#f64 = new Float64Array(this.#rawBuf);
	#u32 = new Uint32Array(this.#rawBuf);
	#byteOffset = 0;
	#retained : any[] = [];

	#flushHandle : NodeJS.Immediate | null = null;
	#flushBound = () => this.#flush(true);

	#currentState : Render2DState = new Render2DState();
	#savedState : Render2DState = new Render2DState();
	#owner : Canvas;
	#transformDirty = false;
	//#endregion

	//#region Lifecycle & Context Management
	constructor(iptr : Pointer | null, cnv : Canvas, args: any, token: {}) {
		if (token !== INTERNAL_TOKEN) throwIllegalConstructor(this)
		const { desynchronized = false, alpha = true } = args ?? {};
		const renderContextPtr = lib.symbols.canvas_get_context2D(iptr, desynchronized, alpha)
		this.#owner = cnv
		
		ptrs.set(this, renderContextPtr)
	}

	get canvas() : Canvas {
		return this.#owner
	}

	reset(token? : {}, w? : number ,h? : number){
		if (token === INTERNAL_TOKEN && w && h ) {
			let off = this.#alloc(16);
		
			this.#i32[off >> 2] = CanvasBatchOperationType.Resize;
			this.#i32[(off + 4) >> 2] = Number(w);
			this.#i32[(off + 8) >> 2] = Number(h);

			this.#scheduleFlush();
		}
		this.#savedState = new Render2DState();
		this.#currentState = new Render2DState();
	}
	//#endregion

	//#region Private Batching & Internal Helpers
	#alloc(bytes: number): number {
		let off = (this.#byteOffset + 7) & ~7;
		if (off + bytes > this.#rawBuf.byteLength) {
			this.#flush(false);
			off = 0;

			// Grow buffer for future frames if we hit the ceiling
			if (this.#rawBuf.byteLength < CanvasRenderingContext2D.#MAX_BUF_SIZE) {
				console.log("increase!")

				this.#rawBuf = new ArrayBuffer(this.#rawBuf.byteLength * 2);
				this.#i32 = new Int32Array(this.#rawBuf);
				this.#f32 = new Float32Array(this.#rawBuf);
				this.#f64 = new Float64Array(this.#rawBuf);
				this.#u32 = new Uint32Array(this.#rawBuf);
			}
		}
		this.#byteOffset = off + bytes;
		return off;
	}

	#flush(commit = false) {
		if (this.#flushHandle !== null) {
			clearImmediate(this.#flushHandle);
			this.#flushHandle = null;
		}
		if (this.#byteOffset === 0) return;

		lib.symbols.canvas_submit_batch(
			ptrs.get(this),
			ptr(this.#rawBuf),
			this.#byteOffset,
			commit
		);

		this.#byteOffset = 0;
		this.#retained.length = 0;
	}

	#scheduleFlush() {
		if (this.#flushHandle === null) {
			this.#flushHandle = setImmediate(this.#flushBound);
		}
	}

	// TypeScript: write 16 float32s (64 bytes) + 8-byte header = 72 bytes inline into #rawBuf
	#uploadTransform() {
		const src = getInternalArray(this.#currentState.transform)!;
		const off = this.#alloc(68); // 8-byte header (4B op + 4B pad) + 16 * 4B floats
		this.#i32[off >> 2] = CanvasBatchOperationType.UploadTransformNoArray;
		for (let i = 0 ; i < 16 ; i++) {
			this.#f32[(off + (i+1)*4) >> 2] = src[i];
		}
		
		// this.#u64[(off + 8) >> 3] = BigInt(ptr(f32))

		this.#scheduleFlush();
		this.#transformDirty = false;
	}

	#setStyleColor(style: string, isStroke : boolean) {
		this.#currentState.fillStyle = style;
		const encoded = encodeCString(style)

		let off = this.#alloc(16);

		const operation = !isStroke
			? CanvasBatchOperationType.SetFillStyleColor
			: CanvasBatchOperationType.SetStrokeStyleColor
		
		this.#i32[off >> 2] = operation;
		writePtrU64(this.#u32, (off + 8) >> 2, ptr(encoded))
		// this.#u64[(off + 8) >> 3] = BigInt(ptr(encoded));

		this.#scheduleFlush();
	}

	#setStyleGradient(style: CanvasGradient, isStroke: boolean) {
		const count = style.colors.length;
		const ptrArray = new BigUint64Array(count);

		this.#retained.push(style)
		
		for (let i = 0; i < count; i++) {
			ptrArray[i] = BigInt(ptr(style.colors[i]));
		}

		const offsetBuffer = new Float32Array(style.offsets);
		const operation = !isStroke 
			? CanvasBatchOperationType.SetFillStyleGradient
			: CanvasBatchOperationType.SetStrokeStyleGradient;

		let off = this.#alloc(40);

		this.#i32[off >> 2] = operation;
		this.#i32[(off + 4) >> 2] = style.type;
		writePtrU64(this.#u32, (off + 8) >> 2, ptr(style.points))
		// this.#u64[(off + 8) >> 3] = BigInt(ptr(style.points));
		this.#i32[(off + 16) >> 2] = count;
		writePtrU64(this.#u32, (off + 24) >> 2, ptr(ptrArray))
		writePtrU64(this.#u32, (off + 32) >> 2, ptr(offsetBuffer))
		// this.#u64[(off + 24) >> 3] = BigInt(ptr(ptrArray));
		// this.#u64[(off + 32) >> 3] = BigInt(ptr(offsetBuffer));
		this.#scheduleFlush();
	}

	// DRY path Helper for fill, stroke and
	#batchPathOperation(
		opIfPathPresent : CanvasBatchOperationType,
		opIfPathNotPresent : CanvasBatchOperationType,
		path? : Path2D
	){
		this.#retained.push(path)
		let _ptr = path?ptrs.get(path):null;
		if (path instanceof Path2D) {
			let off = this.#alloc(16);
			this.#i32[off >> 2] = opIfPathPresent;
			this.#i32[(off + 4) >> 2] = 1;
			writePtrU64(this.#u32, (off + 8) >> 2, _ptr)
		}else{
			let off = this.#alloc(8);
			this.#i32[off >> 2] = opIfPathNotPresent;
		}
	}
	//#endregion

	//#region State Stack
	save(){
		this.#savedState?.copyFrom(this.#currentState)
		let off = this.#alloc(8);
		this.#i32[off >> 2] = CanvasBatchOperationType.Save;
		this.#scheduleFlush();
	}

	restore(){
		if (!this.#savedState) return
		this.#currentState?.copyFrom(this.#savedState)
		let off = this.#alloc(8);
		this.#i32[off >> 2] = CanvasBatchOperationType.Restore;
		this.#scheduleFlush();
	}
	//#endregion

	//#region Transformations
	translate(x : number,y : number){
		this.#currentState.transform.translateSelf(x,y)
		this.#transformDirty = true;
	}

	rotate(deg : number){
		this.#currentState.transform.rotateSelf(deg)
		this.#transformDirty = true;
	}

	scale(x : number, y : number){
		this.#currentState.transform.scaleSelf(x,y)
		this.#transformDirty = true;
	}

	getTransform() : DOMMatrix {
		return DOMMatrix.fromMatrix(this.#currentState.transform);
	}

	setTransform(matrix : DOMMatrix) : void;
	setTransform(a : number, b : number,c : number,d : number,e : number,f : number) : void;
	setTransform(...args : any) : void {
		if (args[0] instanceof DOMMatrix) {
			getInternalArray(this.#currentState.transform)!.set(getInternalArray(args[0])!)
			// this.#currentState.transform = args[0]
			this.#transformDirty = true;
		}
		if (args.length == 6 || args.length == 16) {
			const mtx = new DOMMatrix(args);
			getInternalArray(this.#currentState.transform)!.set(getInternalArray(mtx)!)
			this.#transformDirty = true;
		}
	}

	transform(a : number, b : number, c : number, d : number, e : number, f : number){
		const tf = this.getTransform();
		const mul = new DOMMatrix([a,b,c,d,e,f]);
		tf.multiplySelf(mul)
		this.setTransform(tf)
	}

	resetTransform() : void {
		getInternalArray(this.#currentState.transform)!.set([
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		])
		this.#transformDirty = true;
		// lib.symbols.canvas_reset_transform(ptrs.get(this))
	}
	//#endregion

	//#region Compositing, Filters & Image Smoothing
	set globalAlpha(a : number) {
		this.#currentState.globalAlpha = Math.min(Math.max(a,0),1)
		let off = this.#alloc(16);
		this.#i32[off >> 2] = CanvasBatchOperationType.SetGlobalAlpha;
		this.#f32[(off + 8) >> 2] = this.#currentState.globalAlpha;
		this.#scheduleFlush();
	}
	get globalAlpha() {
		return this.#currentState.globalAlpha
	}

	set globalCompositeOperation(operation : CanvasBlendModeArg) {
		const composite = CanvasBlendMode[operation]
		if (composite) {
			this.#currentState.compositeOperation = operation;
			let off = this.#alloc(16);
			this.#i32[off >> 2] = CanvasBatchOperationType.SetGlobalCompositeOperation;
			this.#f32[(off + 8) >> 2] = composite;
			this.#scheduleFlush();
		}
	}
	get globalCompositeOperation() : string {
		return this.#currentState.compositeOperation as string
	}

	set filter(v : string) {
		if (lib.symbols.canvas_set_filter(ptrs.get(this),encoder.encode(`${v}\0`))){
			this.#currentState.filter = v;
		}
	}
	get filter() : string {
		return this.#currentState.filter;
	}

	set imageSmoothingEnabled(v : boolean) {
		if (lib.symbols.canvas_set_image_smoothing_enabled(ptrs.get(this),v)){
			this.#currentState.imageSmoothingEnabled = v
		}
	}
	get imageSmoothingEnabled() : boolean {
		return this.#currentState.imageSmoothingEnabled;
	}

	set imageSmoothingQuality(v : CanvasSmoothingQualityArg) {
		let smQuality = CanvasSmoothingQuality[v];
		if (lib.symbols.canvas_set_image_smoothing_quality(ptrs.get(this),smQuality)) {
			this.#currentState.imageSmoothingQuality = v;
		}
	}
	get imageSmoothingQuality() : string {
		return this.#currentState.imageSmoothingQuality;
	}
	//#endregion

	//#region Fill, Stroke & Gradient Styles
	set fillStyle(style : string | CanvasGradient) {
		if (style == this.#currentState.fillStyle) return;
		
		if (typeof style === "string") {
			this.#setStyleColor(style,false)
		} else if (style instanceof CanvasGradient) {
			this.#setStyleGradient(style, false);
		}
		// let res = false;
		// if (typeof style === "string") {
		// 	res = lib.symbols.canvas_set_fill_style(ptrs.get(this),encodeCString(style))
		// } else if (style instanceof CanvasGradient) {
			// res = this.#setStyleGradient(style, false);
		// }
		// if (res) this.#currentState.fillStyle = style;
	}
	get fillStyle(){
		return this.#currentState.fillStyle
	}

	set strokeStyle(style : string | CanvasGradient) {
		if (typeof style === "string") {
			this.#setStyleColor(style,true)
		} else if (style instanceof CanvasGradient) {
			this.#setStyleGradient(style, true);
		}
	}
	get strokeStyle(){
		return this.#currentState.strokeStyle
	}

	createConicGradient(a : number, x1 : number, y1 : number) : CanvasGradient;
	createConicGradient(...args : [number,number,number]) : CanvasGradient {
		if (args.length < 3) throw new TypeError(`Failed to execute 'createLinearGradient' on 'CanvasRenderingContext2D': 3 arguments required, but only ${args.length} present.`)
		return new CanvasGradient(INTERNAL_TOKEN,GradientType.Conic, ...args);
	}

	createLinearGradient(x1 : number, y1 : number, x2 : number, y2 : number) : CanvasGradient;
	createLinearGradient(...args : [number,number,number,number]) : CanvasGradient {
		if (args.length < 4) throw new TypeError(`Failed to execute 'createLinearGradient' on 'CanvasRenderingContext2D': 4 arguments required, but only ${args.length} present.`)
		return new CanvasGradient(INTERNAL_TOKEN,GradientType.Linear, ...args);
	}

	createRadialGradient(x1 : number, y1 : number, x2 : number, y2 : number, r0 : number, r1 : number) : CanvasGradient;
	createRadialGradient(...args : [number,number,number,number,number,number]) : CanvasGradient {
		if (args.length < 6) throw new TypeError(`Failed to execute 'createLinearGradient' on 'CanvasRenderingContext2D': 6 arguments required, but only ${args.length} present.`)
		return new CanvasGradient(INTERNAL_TOKEN,GradientType.Radial, ...args);
	}
	//#endregion

	//#region Line Styles
	set lineWidth(width : number) {
		let w = width<1?1:width;
		lib.symbols.canvas_set_stroke_width(ptrs.get(this),w)
		this.#currentState.lineWidth = w
	}
	get lineWidth() : number {
		return this.#currentState.lineWidth
	}

	set lineJoin(joinType : CanvasLineJoinArg){
		let ljType = CanvasLineJoin[joinType];
		if (lib.symbols.canvas_set_line_join(ptrs.get(this),ljType)) {
			this.#currentState.lineJoin = joinType
		}
	}
	get lineJoin() : string{
		return this.#currentState.lineJoin
	}

	set lineDashOffset(v : number){
		if (this.#currentState.lineDash.length <= 0) {
			this.#currentState.lineDashOffset = 0;
			return
		}
		const arr = new Float32Array(this.#currentState.lineDash);
		lib.symbols.canvas_set_line_dash(ptrs.get(this),ptr(arr),arr.length,v)
		this.#currentState.lineDashOffset = v;
	}

	setLineDash(lines : number[]){
		if (lines.constructor.name.toLowerCase().includes("array")){
			let _a
			if (lines.length&1) {
				_a = lines.concat(Array.from(lines))
			}else {
				_a = lines;
			}
			const arr = new Float32Array(_a);
			if(arr.some(Number.isNaN)) return;
			lib.symbols.canvas_set_line_dash(ptrs.get(this),ptr(arr),arr.length,this.#currentState.lineDashOffset)
			this.#currentState.lineDash = _a;
			return;
		}
		
		throw new TypeError(`Failed to execute 'setLineDash' on 'CanvasRenderingContext2D': The provided value cannot be converted to a sequence.`)
	}

	getLineDash(){
		return this.#currentState.lineDash;
	}
	//#endregion

	//#region Shadows
	set shadowBlur(blur : number){
		lib.symbols.canvas_set_shadow_blur(ptrs.get(this),blur)
	}

	set shadowColor(col : string){
		lib.symbols.canvas_set_shadow_color(ptrs.get(this),encoder.encode(`${col}\0`))
	}

	set shadowOffsetX(x : number){
		lib.symbols.canvas_set_shadow_offsetX(ptrs.get(this),x)
	}

	set shadowOffsetY(y : number){
		lib.symbols.canvas_set_shadow_offsetY(ptrs.get(this),y)
	}
	//#endregion

	//#region Rectangles
	fillRect(x : number,y : number,w : number,h : number){
		// lib.symbols.canvas_fill_rect(ptrs.get(this),x,y,w,h)
		if (this.#transformDirty) this.#uploadTransform()

		// Ensure 8-byte alignment for 64-bit pointers & doubles
		let off = this.#alloc(40);

		this.#i32[off >> 2] = CanvasBatchOperationType.FillRect;
		this.#f64[(off + 8) >> 3] = x;
		this.#f64[(off + 16) >> 3] = y;
		this.#f64[(off + 24) >> 3] = w;
		this.#f64[(off + 32) >> 3] = h;

		// this.#byteOffset = off + 40;
		this.#scheduleFlush();
	}

	strokeRect(x : number,y : number,w : number,h : number){
		if (this.#transformDirty) this.#uploadTransform()
		// Ensure 8-byte alignment for 64-bit pointers & doubles
		let off = this.#alloc(40);

		this.#i32[off >> 2] = CanvasBatchOperationType.StrokeRect;
		this.#f64[(off + 8) >> 3] = x;
		this.#f64[(off + 16) >> 3] = y;
		this.#f64[(off + 24) >> 3] = w;
		this.#f64[(off + 32) >> 3] = h;
		this.#scheduleFlush();
	}

	clearRect(x : number, y : number, w : number, h : number){
		// lib.symbols.canvas_clear_rect(ptrs.get(this),x,y,w,h)
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(40);

		this.#i32[off >> 2] = CanvasBatchOperationType.ClearRect;
		this.#f64[(off + 8) >> 3] = x;
		this.#f64[(off + 16) >> 3] = y;
		this.#f64[(off + 24) >> 3] = w;
		this.#f64[(off + 32) >> 3] = h;

		// this.#byteOffset = off + 40;
		this.#scheduleFlush();
	}
	//#endregion

	//#region Paths & Clipping
	beginPath(){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(8);
		this.#i32[off >> 2] = CanvasBatchOperationType.BeginPath;
		this.#scheduleFlush();
	}

	closePath(){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(8);
		this.#i32[off >> 2] = CanvasBatchOperationType.ClosePath;
		this.#scheduleFlush();
	}

	moveTo(x : number,y : number){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(24);
		this.#i32[off >> 2] = CanvasBatchOperationType.MoveTo;
		this.#f64[(off + 8) >> 3] = x;
		this.#f64[(off + 16) >> 3] = y;
		this.#scheduleFlush();
	}

	lineTo(x : number,y : number){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(24);
		this.#i32[off >> 2] = CanvasBatchOperationType.LineTo;
		this.#f64[(off + 8) >> 3] = x;
		this.#f64[(off + 16) >> 3] = y;
		this.#scheduleFlush();
	}

	bezierCurveTo(cp1x : number, cp1y : number, cp2x : number, cp2y : number, x : number, y : number){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(56);
		this.#i32[off >> 2] = CanvasBatchOperationType.BezierCurveTo;
		this.#f64[(off + 8) >> 3] = cp1x
		this.#f64[(off + 16) >> 3] = cp1y
		this.#f64[(off + 24) >> 3] = cp2x
		this.#f64[(off + 32) >> 3] = cp2y
		this.#f64[(off + 40) >> 3] = x
		this.#f64[(off + 48) >> 3] = y
		this.#scheduleFlush();
		// lib.symbols.canvas_path_bezier_to(ptrs.get(this),cp1x, cp1y, cp2x, cp2y, x, y)
	}

	quadraticCurveTo(cpx : number, cpy : number, x : number, y : number){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(40);
		this.#i32[off >> 2] = CanvasBatchOperationType.QuadraticCurveTo;
		this.#f64[(off + 8) >> 3] = cpx
		this.#f64[(off + 16) >> 3] = cpy
		this.#f64[(off + 24) >> 3] = x
		this.#f64[(off + 32) >> 3] = y
		this.#scheduleFlush();
		// lib.symbols.canvas_path_quad_to(ptrs.get(this),cpx,cpy, x, y)
	}

	arc(x : number, y : number, radius : number, startAngle : number, endAngle : number){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(32);
		this.#i32[off >> 2] = CanvasBatchOperationType.Arc;
		this.#f32[(off + 8) >> 2] = x
		this.#f32[(off + 12) >> 2] = y
		this.#f32[(off + 16) >> 2] = radius
		this.#f32[(off + 20) >> 2] = startAngle
		this.#f32[(off + 24) >> 2] = endAngle
		this.#scheduleFlush();
		// lib.symbols.canvas_path_arc(ptrs.get(this),x, y, radius, startAngle, endAngle)
	}

	arcTo(x1 : number, y1 : number, x2 : number, y2 : number, radius : number){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(48);
		this.#i32[off >> 2] = CanvasBatchOperationType.ArcTo;
		this.#f64[(off + 8) >> 3] = x1
		this.#f64[(off + 16) >> 3] = y1
		this.#f64[(off + 24) >> 3] = x2
		this.#f64[(off + 32) >> 3] = y2
		this.#f64[(off + 40) >> 3] = radius
		this.#scheduleFlush();
		// lib.symbols.canvas_path_arc_to(ptrs.get(this),x1, y1, x2, y2, radius)
	}

	ellipse(x : number, y : number, radiusX : number, radiusY : number, rotation : number, startAngle : number, endAngle : number, counterclockwise : boolean = false){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(64);
		this.#i32[off >> 2] = CanvasBatchOperationType.Ellipse;
		this.#i32[(off + 4) >> 2] = Number(counterclockwise)
		this.#f64[(off + 8) >> 3] = x
		this.#f64[(off + 16) >> 3] = y
		this.#f64[(off + 24) >> 3] = radiusX
		this.#f64[(off + 32) >> 3] = radiusY
		this.#f64[(off + 40) >> 3] = rotation
		this.#f64[(off + 48) >> 3] = startAngle
		this.#f64[(off + 56) >> 3] = endAngle
		this.#scheduleFlush();
		// lib.symbols.canvas_path_ellipse(ptrs.get(this),x, y, radiusX, radiusY, rotation, startAngle, endAngle, counterclockwise)
	}

	rect(x : number, y : number, width : number, height : number){
		if (this.#transformDirty) this.#uploadTransform()
		let off = this.#alloc(40);
		this.#i32[off >> 2] = CanvasBatchOperationType.Rect;
		this.#f64[(off + 8) >> 3] = x
		this.#f64[(off + 16) >> 3] = y
		this.#f64[(off + 24) >> 3] = width
		this.#f64[(off + 32) >> 3] = height
		this.#scheduleFlush();
		// lib.symbols.canvas_path_rect(ptrs.get(this),x,y,width,height)
	}

	roundRect(x : number, y : number, width : number, height : number, radii: any){
		if (radii.constructor.name.toLowerCase().includes("array")){
			// const arr = new Float32Array(radii);
			if(radii.some(Number.isNaN)) return;
			if (this.#transformDirty) this.#uploadTransform()

			let off = this.#alloc(56);
			const len = radii.length;

			this.#i32[off >> 2] = CanvasBatchOperationType.RoundRectArray;
			this.#i32[(off + 4) >> 2] = len
			this.#f64[(off + 8) >> 3] = x
			this.#f64[(off + 16) >> 3] = y
			this.#f64[(off + 24) >> 3] = width
			this.#f64[(off + 32) >> 3] = height

			this.#f32[(off + 40) >> 2] = radii[0] ?? 0;
			this.#f32[(off + 44) >> 2] = radii[1] ?? 0;
			this.#f32[(off + 48) >> 2] = radii[2] ?? 0;
			this.#f32[(off + 52) >> 2] = radii[3] ?? 0;
			// writePtrU64(this.#u32, (off + 40) >> 2, ptr(arr))
			// this.#u64[(off + 40) >> 3] = BigInt(ptr(arr))
			this.#scheduleFlush();

			// lib.symbols.canvas_path_roundrect_array(ptrs.get(this),x,y,width,height,ptr(arr), len)
			return;
		} else {
			if (this.#transformDirty) this.#uploadTransform()
			let off = this.#alloc(48);

			this.#i32[off >> 2] = CanvasBatchOperationType.RoundRect;
			this.#f64[(off + 8) >> 3] = x
			this.#f64[(off + 16) >> 3] = y
			this.#f64[(off + 24) >> 3] = width
			this.#f64[(off + 32) >> 3] = height
			this.#f64[(off + 40) >> 3] = radii
			this.#scheduleFlush();
			// lib.symbols.canvas_path_roundrect(ptrs.get(this),x,y,width,height,radii)
		}
	}

	fill(path? : Path2D){
		this.#batchPathOperation(
			CanvasBatchOperationType.FillPath,
			CanvasBatchOperationType.Fill,
			path
		)
	}

	stroke(path? : Path2D){
		this.#batchPathOperation(
			CanvasBatchOperationType.StrokePath,
			CanvasBatchOperationType.Stroke,
			path
		)
	}

	clip(path? : Path2D){
		this.#batchPathOperation(
			CanvasBatchOperationType.ClipPath,
			CanvasBatchOperationType.Clip,
			path
		)
	}
	//#endregion

	//#region Text & Typography
	set font(css: string) {
		// The native side will parse the CSS string, resolve a typeface and cache it.
		const normalized = css.trim().toLowerCase()
		const encoded = encodeCString(normalized);

		let off = this.#alloc(16);
		this.#i32[off >> 2] = CanvasBatchOperationType.SetFont;
		// this.#u64[(off + 8) >> 3] = BigInt(ptr(encoded));
		writePtrU64(this.#u32, (off + 8) >> 2, ptr(encoded))

		this.#currentState.fontCss = normalized;
		this.#scheduleFlush();
	}
	get font(): string { 
		return this.#currentState.fontCss;
	}

	set textAlign(alignment : CanvasTextAlignmentArg){
		const al = CanvasTextAlignment[alignment];
		if (!al) return;
		let off = this.#alloc(8);
		this.#i32[off >> 2] = CanvasBatchOperationType.SetTextAlign;
		this.#i32[(off + 4) >> 2] = al;
		this.#currentState.textAlign = alignment
		this.#scheduleFlush();
	}

	set textBaseline(baseline : CanvasTextBaselineArg){
		const bl = CanvasTextBaseline[baseline]
		if (!bl) return;
		let off = this.#alloc(8);
		this.#i32[off >> 2] = CanvasBatchOperationType.SetTextBaseline;
		this.#i32[(off + 4) >> 2] = bl;
		this.#currentState.textBaseline = baseline
		this.#scheduleFlush();
	}

	fillText(text:string,x:number,y:number, maxWidth = -1){
		if (this.#transformDirty) this.#uploadTransform()
		const encoded = encoder.encode(`${text}\0`);
		this.#retained.push(encoded)
		let off = this.#alloc(40);

		this.#i32[off >> 2] = CanvasBatchOperationType.FillText;
		this.#i32[(off + 4) >> 2] = text.length;
		writePtrU64(this.#u32, (off + 8) >> 2, ptr(encoded))
		// this.#u64[(off + 8) >> 3] = BigInt(ptr(encoded));
		this.#f64[(off + 16) >> 3] = x;
		this.#f64[(off + 24) >> 3] = y;
		this.#f64[(off + 32) >> 3] = maxWidth;
		this.#scheduleFlush();
		// lib.symbols.canvas_fill_text(ptrs.get(this),encoder.encode(`${text}\0`),x,y,maxWidth)
	}

	strokeText(text:string,x:number,y:number, maxWidth = -1){
		if (this.#transformDirty) this.#uploadTransform()
		const encoded = encoder.encode(`${text}\0`);
		this.#retained.push(encoded)
		let off = this.#alloc(40);

		this.#i32[off >> 2] = CanvasBatchOperationType.StrokeText;
		writePtrU64(this.#u32, (off + 8) >> 2, ptr(encoded))
		// this.#u64[(off + 8) >> 3] = BigInt(ptr(encoded));
		this.#f64[(off + 16) >> 3] = x;
		this.#f64[(off + 24) >> 3] = y;
		this.#f64[(off + 32) >> 3] = maxWidth;
		// lib.symbols.canvas_stroke_text(ptrs.get(this),encoder.encode(`${text}\0`),x,y,maxWidth)
	}

	measureText(text : string) : TextMetrics {
		if (this.#transformDirty) this.#uploadTransform()
		const pArr = new Float32Array(12)
		lib.symbols.canvas_measure_text(ptrs.get(this),encoder.encode(`${text}\0`),ptr(pArr))
		return new TextMetrics(
			INTERNAL_TOKEN,
			pArr[0], 
			pArr[1], 
			pArr[2], 
			pArr[3], 
			pArr[4], 
			pArr[5], 
			pArr[6], 
			pArr[7],
			pArr[8],
			pArr[9], 
			pArr[10], 
			pArr[11]
		)
	}
	//#endregion

	//#region Images & Pixel Manipulation
	drawImage(img : Image | Canvas, x : number, y : number, w : number,h : number) : void;
	drawImage(img : Image | Canvas, sx : number, sy : number, sw : number, sh : number, dx : number, dy : number, dw : number, dh : number) : void;
	drawImage(img : Image | Canvas, ...args : Array<number>) {
		let _arr : number[] | Float32Array = args.length==2?[...args,img.width,img.height]:args;
		const arr = new Float32Array(_arr)
		const len = arr.length
		if (img instanceof Image) {
			if (this.#transformDirty) this.#uploadTransform()
			lib.symbols.canvas_draw_image_imageType(ptrs.get(this),img.ptr,ptr(arr),len)
			return;
		}
		if (img instanceof Canvas) {
			if (this.#transformDirty) this.#uploadTransform()
			lib.symbols.canvas_draw_image_canvasType(ptrs.get(this),ptrs.get(img),ptr(arr),len)
			return;
		}
		
		throw new TypeError("Failed to execute 'drawImage' on 'CanvasRenderingContext2D': The provided value is not of type '(Image or Canvas)'.")
	}

	getImageData(x: number, y: number, w: number, h: number): ImageData {
		const arr = new Uint8ClampedArray(w * h * 4);
		lib.symbols.canvas_get_image_data(ptrs.get(this), x, y, w, h, ptr(arr));
		return new ImageData(arr, w, h);
	}

	putImageData(
		data: ImageData,
		dx: number,
		dy: number,
		dirtyX = 0,
		dirtyY = 0,
		dirtyWidth = data.width,
		dirtyHeight = data.height
	) {
		if (!(data instanceof ImageData)) {
			throw new TypeError(
				"Failed to execute 'putImageData' on 'CanvasRenderingContext2D': " +
				"parameter 1 is not of type 'ImageData'"
			);
		}

		lib.symbols.canvas_put_image_data(
			ptrs.get(this),
			dx,
			dy,
			data.width,
			data.height,
			dirtyX,
			dirtyY,
			dirtyWidth,
			dirtyHeight,
			ptr(data.data)
		);
	}
	//#endregion
}

{
	const proto = CanvasRenderingContext2D.prototype;
	
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

export class Canvas {
	#dim = new Float64Array(2);

	#ctx : CanvasRenderingContext2D | null = null;
	constructor(w=1,h=1, options? : { gpu : boolean }) {
		this.#dim[0] = w;
		this.#dim[1] = h;

		let ptr : bigint | Pointer | null; 

		if (Bun.isMainThread) {
			ptr = lib.symbols.canvas_create(w,h,options?options.gpu:true);
		} else {
			ptr = lib.symbols.canvas_create(w,h,false);
		}

		
		ptrs.set(this,ptr)
		canvasFinalizationRegistry.register(this,ptr)
	}
	
	set width(v : number){
		this.#dim[0] = v<1?1:v;
		if (this.#ctx) {
			this.#ctx.reset(INTERNAL_TOKEN, this.#dim[0], this.#dim[1])
		}
	}
	set height(v : number){
		this.#dim[1] = v<1?1:v
		// lib.symbols.canvas_resize(ptrs.get(this),this.#dim[0],this.#dim[1])
		if (this.#ctx) {
			this.#ctx.reset(INTERNAL_TOKEN, this.#dim[0], this.#dim[1])
		}
	}
	get width() : number{
		return this.#dim[0]
	}
	get height() : number {
		return this.#dim[1]
	}
	
	getContext(contextType : string, args : any) {
		if (contextType == "2d") {
			if (this.#ctx) return this.#ctx
			this.#ctx = new CanvasRenderingContext2D(ptrs.get(this),this, args, INTERNAL_TOKEN)
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
