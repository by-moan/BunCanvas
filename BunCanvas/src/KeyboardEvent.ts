export type KeyboardEventKey = "Unknown" | " " | "'" | "," | "-" | "." | "/" | "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | ";" | "=" | "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x" | "y" | "z" | "[" | "\\" | "]" | "`" | "World1" | "World2" | "Escape" | "Enter" | "Tab" | "Backspace" | "Insert" | "Delete" | "ArrowRight" | "ArrowLeft" | "ArrowDown" | "ArrowUp" | "PageUp" | "PageDown" | "Home" | "End" | "CapsLock" | "ScrollLock" | "NumLock" | "PrintScreen" | "Pause" | "F1" | "F2" | "F3" | "F4" | "F5" | "F6" | "F7" | "F8" | "F9" | "F10" | "F11" | "F12" | "F13" | "F14" | "F15" | "F16" | "F17" | "F18" | "F19" | "F20" | "F21" | "F22" | "F23" | "F24" | "F25" | "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "." | "/" | "*" | "-" | "+" | "Enter" | "=" | "Shift" | "Control" | "Alt" | "Meta" | "Control" | "Alt" | "Meta" | "ContextMenu" | "\"" | "<" | "_" | ">" | "?" | ")" | "!" | "@" | "#" | "$" | "%" | "^" | "&" | "*" | "(" | ":" | "+" | "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z" | "{" | "|" | "}" | "~" | "World1" | "World2" | "Escape" | "Enter" | "Tab" | "Backspace" | "Insert" | "Delete" | "ArrowRight" | "ArrowLeft" | "ArrowDown" | "ArrowUp" | "PageUp" | "PageDown" | "Home" | "End" | "CapsLock" | "ScrollLock" | "NumLock" | "PrintScreen" | "Pause" | "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "." | "/" | "*" | "-" | "+" | "Enter" | "=" | "Control" | "Alt" | "Meta" | "Control" | "Alt" | "Meta" | "ContextMenu"
export type KeyboardEventCode = "Unknown" | "Space" | "Quote" | "Comma" | "Minus" | "Period" | "Slash" | "Digit0" | "Digit1" | "Digit2" | "Digit3" | "Digit4" | "Digit5" | "Digit6" | "Digit7" | "Digit8" | "Digit9" | "Semicolon" | "Equal" | "KeyA" | "KeyB" | "KeyC" | "KeyD" | "KeyE" | "KeyF" | "KeyG" | "KeyH" | "KeyI" | "KeyJ" | "KeyK" | "KeyL" | "KeyM" | "KeyN" | "KeyO" | "KeyP" | "KeyQ" | "KeyR" | "KeyS" | "KeyT" | "KeyU" | "KeyV" | "KeyW" | "KeyX" | "KeyY" | "KeyZ" | "BracketLeft" | "Backslash" | "BracketRight" | "Backquote" | "IntlBackslash" | "Escape" | "Enter" | "Tab" | "Backspace" | "Insert" | "Delete" | "ArrowRight" | "ArrowLeft" | "ArrowDown" | "ArrowUp" | "PageUp" | "PageDown" | "Home" | "End" | "CapsLock" | "ScrollLock" | "NumLock" | "PrintScreen" | "Pause" | "F1" | "F2" | "F3" | "F4" | "F5" | "F6" | "F7" | "F8" | "F9" | "F10" | "F11" | "F12" | "F13" | "F14" | "F15" | "F16" | "F17" | "F18" | "F19" | "F20" | "F21" | "F22" | "F23" | "F24" | "F25" | "Numpad0" | "Numpad1" | "Numpad2" | "Numpad3" | "Numpad4" | "Numpad5" | "Numpad6" | "Numpad7" | "Numpad8" | "Numpad9" | "NumpadDecimal" | "NumpadDivide" | "NumpadMultiply" | "NumpadSubtract" | "NumpadAdd" | "NumpadEnter" | "NumpadEqual" | "ShiftLeft" | "ControlLeft" | "AltLeft" | "MetaLeft" | "ShiftRight" | "ControlRight" | "AltRight" | "MetaRight" | "ContextMenu"

export interface KeyboardEventInit extends EventInit {
	// UIEventInit properties
	detail?: number;
	view?: Window | null;
	
	// EventModifierInit properties
	altKey?: boolean;
	ctrlKey?: boolean;
	metaKey?: boolean;
	shiftKey?: boolean;
	modifierAltGraph?: boolean;
	modifierCapsLock?: boolean;
	modifierFn?: boolean;
	modifierFnLock?: boolean;
	modifierHyper?: boolean;
	modifierNumLock?: boolean;
	modifierScrollLock?: boolean;
	modifierSuper?: boolean;
	modifierSymbol?: boolean;
	modifierSymbolLock?: boolean;
	
	// KeyboardEventInit properties
	key?: KeyboardEventKey;
	code?: KeyboardEventCode;
	location?: number;
	repeat?: boolean;
	isComposing?: boolean;
}

export class KeyboardEvent extends Event {
	// Standard W3C Key Location constants
	static readonly DOM_KEY_LOCATION_STANDARD = 0x00;
	static readonly DOM_KEY_LOCATION_LEFT = 0x01;
	static readonly DOM_KEY_LOCATION_RIGHT = 0x02;
	static readonly DOM_KEY_LOCATION_NUMPAD = 0x03;
	
	// Private backing fields for runtime immutability
	#altKey: boolean;
	#code: KeyboardEventCode;
	#ctrlKey: boolean;
	#detail: number;
	#isComposing: boolean;
	#key: KeyboardEventKey;
	#location: number;
	#metaKey: boolean;
	#repeat: boolean;
	#shiftKey: boolean;
	#view: Window | null;
	#modifiers: Map<string, boolean>;
	
	constructor(type: string, eventInitDict: KeyboardEventInit = {}) {
		super(type, {
			bubbles: eventInitDict.bubbles ?? false,
			cancelable: eventInitDict.cancelable ?? false,
			composed: eventInitDict.composed ?? false,
		});
		
		this.#altKey = Boolean(eventInitDict.altKey);
		this.#code = String(eventInitDict.code ?? "") as KeyboardEventCode;
		this.#ctrlKey = Boolean(eventInitDict.ctrlKey);
		this.#detail = Number(eventInitDict.detail ?? 0);
		this.#isComposing = Boolean(eventInitDict.isComposing);
		this.#key = String(eventInitDict.key ?? "") as KeyboardEventKey;
		this.#location = Number(eventInitDict.location ?? KeyboardEvent.DOM_KEY_LOCATION_STANDARD);
		this.#metaKey = Boolean(eventInitDict.metaKey);
		this.#repeat = Boolean(eventInitDict.repeat);
		this.#shiftKey = Boolean(eventInitDict.shiftKey);
		this.#view = eventInitDict.view ?? null;
		
		this.#modifiers = new Map<string, boolean>([
			["Alt", this.#altKey],
			["Control", this.#ctrlKey],
			["Meta", this.#metaKey],
			["Shift", this.#shiftKey],
			["AltGraph", Boolean(eventInitDict.modifierAltGraph)],
			["CapsLock", Boolean(eventInitDict.modifierCapsLock)],
			["Fn", Boolean(eventInitDict.modifierFn)],
			["FnLock", Boolean(eventInitDict.modifierFnLock)],
			["Hyper", Boolean(eventInitDict.modifierHyper)],
			["NumLock", Boolean(eventInitDict.modifierNumLock)],
			["ScrollLock", Boolean(eventInitDict.modifierScrollLock)],
			["Super", Boolean(eventInitDict.modifierSuper)],
			["Symbol", Boolean(eventInitDict.modifierSymbol)],
			["SymbolLock", Boolean(eventInitDict.modifierSymbolLock)],
		]);
	}
	
	// Standard KeyboardEvent readonly getters
	get altKey(): boolean {
		return this.#altKey;
	}
	
	get code(): KeyboardEventCode {
		return this.#code;
	}
	
	get ctrlKey(): boolean {
		return this.#ctrlKey;
	}
	
	get isComposing(): boolean {
		return this.#isComposing;
	}
	
	get key(): KeyboardEventKey {
		return this.#key;
	}
	
	get location(): number {
		return this.#location;
	}
	
	get metaKey(): boolean {
		return this.#metaKey;
	}
	
	get repeat(): boolean {
		return this.#repeat;
	}
	
	get shiftKey(): boolean {
		return this.#shiftKey;
	}
	
	// UIEvent readonly getters
	get detail(): number {
		return this.#detail;
	}
	
	get view(): Window | null {
		return this.#view;
	}
	
	getModifierState(keyArg: string): boolean {
		return this.#modifiers.get(keyArg) ?? false;
	}
}
