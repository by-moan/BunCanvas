import { MouseEvent } from "./MouseEvent";

export class WheelEvent extends MouseEvent {
        deltaX : number =  0
        deltaY : number =  0
        constructor(type : string, args : any = {altKey:false, button:0, buttons:0, clientX:0, clientY:0, movementX: 0, movementY: 0, deltaX: 0, deltaY: 0, ctrlKey:false, metaKey:false, screenX:0, screenY:0, shiftKey:false, target:null}) {
            super(type, args);
            this.deltaX = args.deltaX ?? 0
            this.deltaY = args.deltaY ?? 0
        }
    }