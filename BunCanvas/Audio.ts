import { encodeCString, encoder, lib } from "./symbols";

const ptrs = new WeakMap();

lib.symbols.audio_backend_init();

export class Audio {
    constructor();
    constructor(path: string);
    constructor(path? : string){
        if (path) {
            ptrs.set(this,lib.symbols.audio_create(encoder.encode(`${path}\0`)))
        }else {
            ptrs.set(this,lib.symbols.audio_create())
        }
    }

    set src(v : string){
        lib.symbols.audio_set_src(encodeCString(v))
    }

    play(){
        lib.symbols.audio_play(ptrs.get(this))
    }
}