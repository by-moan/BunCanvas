class MouseEvent {
    altKey = false
    button = 0
    buttons = 0
    clientX = 0
    clientY = 0
    ctrlKey = false
    metaKey = false
    screenX = 0
    screenY = 0
    shiftKey = false
    type = ""


    constructor(type, args = {altKey:false, button:0, buttons:0, clientX:0, clientY:0, ctrlKey:false, metaKey:false, screenX:0, screenY:0, shiftKey:false}){
        this.type = type
        this.altKey = args.altKey
        this.button = args.button
        this.buttons = args.buttons
        this.clientX = args.clientX
        this.clientY = args.clientY
        this.ctrlKey = args.ctrlKey
        this.metaKey = args.metaKey
        this.screenX = args.screenX
        this.screenY = args.screenY
        this.shiftKey = args.shiftKey
    }
}

