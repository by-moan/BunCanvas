import { Event } from "./Event"
const GLFW_KEY_MAP = {
	"-1": "Unknown",
	32: " ",
	
	91: "[",
	92: "\\",
	93: "]",
	
	256: "Escape",
	257: "Enter",
	258: "Tab",
	259: "Backspace",
	260: "Insert",
	261: "Delete",
	
	262: "ArrowRight",
	263: "ArrowLeft",
	264: "ArrowDown",
	265: "ArrowUp",
	
	266: "PageUp",
	267: "PageDown",
	268: "Home",
	269: "End",
	
	280: "CapsLock",
	281: "ScrollLock",
	282: "NumLock",
	283: "PrintScreen",
	284: "Pause",
	
	290: "F1",
	291: "F2",
	292: "F3",
	293: "F4",
	294: "F5",
	295: "F6",
	296: "F7",
	297: "F8",
	298: "F9",
	299: "F10",
	300: "F11",
	301: "F12",
	302: "F13",
	303: "F14",
	304: "F15",
	305: "F16",
	306: "F17",
	307: "F18",
	308: "F19",
	309: "F20",
	310: "F21",
	311: "F22",
	312: "F23",
	313: "F24",
	314: "F25",
	
	48: "0",
	49: "1",
	50: "2",
	51: "3",
	52: "4",
	53: "5",
	54: "6",
	55: "7",
	56: "8",
	57: "9",
	
	320: "0",
	321: "1",
	322: "2",
	323: "3",
	324: "4",
	325: "5",
	326: "6",
	327: "7",
	328: "8",
	329: "9",
	
	330: ".",
	331: "/",
	332: "*",
	333: "-",
	334: "+",
	335: "Enter",
	336: "=",
	
	340: "Shift",
	341: "Control",
	342: "Alt",
	343: "Meta",
	344: "Shift",
	345: "Control",
	346: "Alt",
	347: "Meta",
	348: "ContextMenu"
};

const GLFW_CODE_MAP = {
	"-1": "Unknown",
	// Printable keys (GLFW mostly uses ASCII here)
	32: "Space",
	39: "Quote",
	44: "Comma",
	45: "Minus",
	46: "Period",
	47: "Slash",
	
	48: "Digit0",
	49: "Digit1",
	50: "Digit2",
	51: "Digit3",
	52: "Digit4",
	53: "Digit5",
	54: "Digit6",
	55: "Digit7",
	56: "Digit8",
	57: "Digit9",
	
	59: "Semicolon",
	61: "Equal",
	
	65: "KeyA",
	66: "KeyB",
	67: "KeyC",
	68: "KeyD",
	69: "KeyE",
	70: "KeyF",
	71: "KeyG",
	72: "KeyH",
	73: "KeyI",
	74: "KeyJ",
	75: "KeyK",
	76: "KeyL",
	77: "KeyM",
	78: "KeyN",
	79: "KeyO",
	80: "KeyP",
	81: "KeyQ",
	82: "KeyR",
	83: "KeyS",
	84: "KeyT",
	85: "KeyU",
	86: "KeyV",
	87: "KeyW",
	88: "KeyX",
	89: "KeyY",
	90: "KeyZ",
	
	91: "BracketLeft",
	92: "Backslash",
	93: "BracketRight",
	96: "Backquote",
	
	// Special
	256: "Escape",
	257: "Enter",
	258: "Tab",
	259: "Backspace",
	260: "Insert",
	261: "Delete",
	262: "ArrowRight",
	263: "ArrowLeft",
	264: "ArrowDown",
	265: "ArrowUp",
	266: "PageUp",
	267: "PageDown",
	268: "Home",
	269: "End",
	
	280: "CapsLock",
	281: "ScrollLock",
	282: "NumLock",
	283: "PrintScreen",
	284: "Pause",
	
	// Function keys
	290: "F1",
	291: "F2",
	292: "F3",
	293: "F4",
	294: "F5",
	295: "F6",
	296: "F7",
	297: "F8",
	298: "F9",
	299: "F10",
	300: "F11",
	301: "F12",
	302: "F13",
	303: "F14",
	304: "F15",
	305: "F16",
	306: "F17",
	307: "F18",
	308: "F19",
	309: "F20",
	310: "F21",
	311: "F22",
	312: "F23",
	313: "F24",
	314: "F25",
	
	// Keypad
	320: "Numpad0",
	321: "Numpad1",
	322: "Numpad2",
	323: "Numpad3",
	324: "Numpad4",
	325: "Numpad5",
	326: "Numpad6",
	327: "Numpad7",
	328: "Numpad8",
	329: "Numpad9",
	330: "NumpadDecimal",
	331: "NumpadDivide",
	332: "NumpadMultiply",
	333: "NumpadSubtract",
	334: "NumpadAdd",
	335: "NumpadEnter",
	336: "NumpadEqual",
	
	// Modifiers
	340: "ShiftLeft",
	341: "ControlLeft",
	342: "AltLeft",
	343: "MetaLeft",
	344: "ShiftRight",
	345: "ControlRight",
	346: "AltRight",
	347: "MetaRight",
	
	348: "ContextMenu"
};

export class KeyboardEvent extends Event {
	target = null
	key = ""
	code = ""
	location = 0
	repeat = false
	ctrlKey = false
	shiftKey = false
	altKey = false
	metaKey = false
	
	
	constructor(type, args = {
		target: null,
		key: "",
		code: "",
		location: 0,
		repeat: false,
		ctrlKey: false,
		shiftKey: false,
		altKey: false,
		metaKey: false
	}, kcode = null, target = null, repeat = false, shiftKey = false, ctrlKey = false, altKey = false, metaKey = false){
		super(type,(args)?args.target:target)
		if (!!args) {
			this.key = args.key
			this.code = args.code
			this.location = args.location
			this.repeat = args.repeat
			this.ctrlKey = args.ctrlKey
			this.shiftKey = args.shiftKey
			this.altKey = args.altKey
			this.metaKey = args.metaKey
		} else {
			const glfwKey = kcode
			if (glfwKey >= 65 && glfwKey <= 90) {
				let c = String.fromCharCode(glfwKey);
				// const uppercase = shift ^ caps;
				this.key = shiftKey ? c : c.toLowerCase();
			}else {
				this.key = GLFW_KEY_MAP[kcode]
				
			}
			this.code = GLFW_CODE_MAP[kcode]
			this.location = 0
			this.repeat = false
			this.shiftKey = shiftKey
			this.ctrlKey = ctrlKey
			this.altKey = altKey
			this.metaKey = metaKey
		}
		
	}
}

