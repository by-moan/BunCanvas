import { encoder, lib } from "./symbols";

const ptrs = new WeakMap();

export class FontFace {
	family;
	weight = "normal";
	style  = "normal";
	stretch = "normal";
	unicodeRange = "U+0-10FFFF";
	variant = "normal";
	featureSettings = "normal";
	display = "auto";
	
	#status = "unloaded";
	get status() { return this.#status; }
	#loadedPromise = null;
	get loaded() { return this.#loadedPromise; }
	
	constructor(family, source, descriptors = {}) {
		this.family = family;
		
		this.weight      = descriptors.weight               ?? "normal";
		this.style       = descriptors.style                ?? "normal";
		this.stretch     = descriptors.stretch              ?? "normal";
		this.unicodeRange= descriptors.unicodeRange         ?? "U+0-10FFFF";
		this.variant     = descriptors.variant              ?? "normal";
		this.featureSettings = descriptors.featureSettings  ?? "normal";
		this.display     = descriptors.display              ?? "auto";
		
		let src = "";
		if (typeof source === "string") {
			const m = source.match(/url$$["']?([^"')]+)["']?$$/);
			console.log(m ? m[1] : source)
			src = m ? m[1] : source;
		} else if (source instanceof ArrayBuffer) {
			throw new Error("ArrayBuffer source not yet supported");
		}
		
		const weightNum = this._cssWeightToNumber(this.weight);
		const styleNum  = this._cssStyleToNumber(this.style);
		const stretchNum= this._cssStretchToNumber(this.stretch);
		
		const ptr = lib.symbols.font_create_from_face(
			encoder.encode(`${this.family.toLowerCase()}\0`),
			encoder.encode(`${src}\0`),
			weightNum,
			styleNum,
			stretchNum,
			encoder.encode(`${this.unicodeRange}\0`),
			encoder.encode(`${this.variant}\0`),
			encoder.encode(`${this.featureSettings}\0`)
		);
		ptrs.set(this, ptr);
	}
	
	// ---- helpers ----------------------------------------------------
	_cssWeightToNumber(w) {
	if (typeof w === "number") return Math.max(1, Math.min(1000, w|0));
	const map = { thin:100, extralight:200, light:300, normal:400, medium:500,
		semibold:600, bold:700, extrabold:800, black:900 };
		return map[w.toLowerCase()] ?? 400;
	}
	_cssStyleToNumber(s) {
		return s === "italic" ? 1 : (s === "oblique" ? 2 : 0);
	}
	_cssStretchToNumber(s) {
		const map = {
			"ultra-condensed":1, "extra-condensed":2, "condensed":3, "semi-condensed":4,
			"normal":5, "semi-expanded":6, "expanded":7, "extra-expanded":8, "ultra-expanded":9
		};
		return map[s.toLowerCase()] ?? 5;
	}
	
	async load() {
		if (this.#status === "loaded") return this;
		if (!this.#loadedPromise) {
			this.#loadedPromise = new Promise((resolve, reject) => {
				const ok = lib.symbols.font_load(ptrs.get(this));
				if (ok) {
					this.#status = "loaded";
					resolve(this);
				} else {
					this.#status = "error";
					reject(new Error("Font load failed"));
				}
			});
		}
		return this.#loadedPromise;
	}
	
	_nativePtr() { return ptrs.get(this); }
}

{
	const proto = FontFace.prototype;
	
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