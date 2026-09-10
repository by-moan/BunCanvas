import { createHash } from "node:crypto";
import { basename, join } from "node:path";
import { tmpdir } from "node:os";
import { lib,encoder } from "../internal/symbols.js";
import { Event } from "./Event.js";
import { ptr } from "bun:ffi"
const ptrs = new WeakMap();
const fRegistry = new FinalizationRegistry((ptr: any)=>{
    lib.symbols.image_destroy(ptr)
	console.log("destroy!")
});

function computePath(url: string) : string {
	const hash = createHash("sha256")
		.update(url)
		.digest("hex")
		.slice(0, 16);

	const parsed = new URL(url);
	const filename = basename(parsed.pathname) || "index";

    return join(tmpdir(), `${hash}-${filename}`);
}

function fetchToTemp(url: string): Promise<string>;
function fetchToTemp(url: string, precomputedPath : string | undefined): Promise<string>;
function fetchToTemp(url: string, precomputedPath? : string): Promise<string> {
    return (async () => {
        let path : string = precomputedPath ?? computePath(url);

		//Tried using Promise<Response> as type, but it still errors
		// So.... cheers ¯\_(ツ)_/¯
        const response : Response = await fetch(url);
        if (!response.ok || !response.body) {
            throw new Error(`HTTP ${response.status}: ${url}`);
        }
		const buf = await response.arrayBuffer();
		// console.log(buf)
        await Bun.write(path, buf);

        return path;
    })();
}

const urlRegex = /^https?:\/\/(?:www\.)?[^\s/$.?#].[^\s]*$/;

export class Image {
	#queue : any = null;

	#width : number = 0;
	#setWidth = false
	#height : number = 0;
	#setHeight = false

	#src : string = "";
	constructor() {
		let ptr = lib.symbols.image_create();
		ptrs.set(this,ptr)
		fRegistry.register(this,ptr)
	}
	
	onload: ((this: Image, evt: Event) => any) | null = null;
	onerror: ((this: Image, evt: Event) => any) | null = null;
	
	set src(path: string){
		this.#src = path;
		if (urlRegex.test(path)) {
			const weakThis = new WeakRef(this);
			const precomp = computePath(path)
			const iptr = ptrs.get(this);
			lib.symbols.image_set_src(iptr, encoder.encode(`${precomp}\0`), true)
			fetchToTemp(path,precomp).then(() => {
				const tArr = new Int32Array(2)
				const self = weakThis.deref();
				if (!self) {
					return
				};
				if (lib.symbols.image_load(iptr,ptr(tArr))) {
					this.#width = !this.#setWidth?tArr[0]:this.#width;
					this.#height = !this.#setHeight?tArr[1]:this.#height;
					self.onload?.call(self, new Event("load", self));
				} else {
					self.onerror?.call(self, new Event("error", self));
				}
				
			}).catch((ex) => {
				const self = weakThis.deref();
				if (!self) return;
				self.onerror?.call(self, new Event("error", self));
			}).finally(()=>{});
			return;
		}else {
			const weakThis = new WeakRef(this);
			lib.symbols.image_set_src(ptrs.get(this), encoder.encode(`${path}\0`), false)
			let tm = setTimeout(() => {
				const self = weakThis.deref()
				
				if (!self) {
					clearTimeout(tm)
					return;
				}
				const _ptr = ptrs.get(self);
				const tArr = new Int32Array(2)
				if (lib.symbols.image_load(_ptr,ptr(tArr))) {
					this.#width = !this.#setWidth?tArr[0]:this.#width;
					this.#height = !this.#setHeight?tArr[1]:this.#height;
					self.onload?.call(self, new Event("load", self));
				} else {
					self.onerror?.call(self, new Event("error", self));
				}
				clearTimeout(tm);
			}, 0);
		}

		// Local file path handling (unchanged)
		
	}

	get src(){
		return this.#src;
	}

    get ptr(){
        return ptrs.get(this);
    }

	get width(){
		return this.#width
	}
	get height(){
		return this.#height
	}

	set width(v : number){
		this.#setWidth = true
		this.#width = v
	}
	set height(v : number){
		this.#setHeight = true
		this.#height = v
	}
}

{
	const proto = Image.prototype;
	
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
