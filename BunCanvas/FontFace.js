import { encoder, lib } from "./symbols";

const ptrs = new WeakMap();

export class FontFace {
	#fontName = ""
	#path = ""

	_nativePtr = null;

	constructor(fontName, path) {
		this.#fontName	= fontName.toLowerCase()
		this.#path		= path.trim().replaceAll('url(', "").replaceAll('"', "").replaceAll("'","").replaceAll(")","").trim()
	}

	get family(){
		return this.#fontName
	}

	async load(){
		return new Promise((resolve,reject)=>{
			this._nativePtr = lib.symbols.font_load(encoder.encode(`${this.#path}\0`))
			console.log(this._nativePtr, this.#path)
			if(this._nativePtr){resolve()}
			else{reject()}
		})
	}
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