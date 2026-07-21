import { lib,encoder } from "./symbols.js";
import { Event } from "./Event.js";

const ptrs = new WeakMap();
const fRegistry = new FinalizationRegistry((ptr: any)=>{
    lib.symbols.image_destroy(ptr);
});

export class Image {
	constructor() {
		let ptr = lib.symbols.image_create();
		ptrs.set(this,ptr)
		fRegistry.register(this,ptr)
	}
	
	onload: ((this: Image, evt: Event) => any) | null = null;
	onerror: ((this: Image, evt: Event) => any) | null = null;
	
	set src(path : string){
		setImmediate(()=>{
			if (lib.symbols.image_set_src(ptrs.get(this), encoder.encode(`${path}\0`))) {
				this.onload instanceof Function ? this.onload(new Event("load",this)):undefined;
			}else {
				this.onerror instanceof Function ? this.onerror(new Event("error",this)):undefined;
			}
		})
	}

    get ptr(){
        return ptrs.get(this);
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
