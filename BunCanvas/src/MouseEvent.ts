import { Event } from "./Event"

let mEvt;


export class MouseEvent extends Event {
        altKey = false
        button = 0
        buttons = 0
        clientX = 0
        clientY = 0
        movementX = 0
        movementY = 0
        ctrlKey = false
        metaKey = false
        screenX = 0
        screenY = 0
        shiftKey = false
        
        constructor(type : string, args : any = {altKey:false, button:0, buttons:0, clientX:0, clientY:0, movementX: 0, movementY: 0, ctrlKey:false, metaKey:false, screenX:0, screenY:0, shiftKey:false, target:null}){
            super(type,args.target)
            this.altKey = args.altKey
            this.button = args.button
            this.buttons = args.buttons
            this.clientX = args.clientX
            this.clientY = args.clientY
            this.movementX = args.movementX
            this.movementY = args.movementY
            this.ctrlKey = args.ctrlKey
            this.metaKey = args.metaKey
            this.screenX = args.screenX
            this.screenY = args.screenY
            this.shiftKey = args.shiftKey
        }
    }
    const proto = MouseEvent.prototype;
        
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