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
	key?: string;
	code?: string;
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
	#code: string;
	#ctrlKey: boolean;
	#detail: number;
	#isComposing: boolean;
	#key: string;
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
		this.#code = String(eventInitDict.code ?? "");
		this.#ctrlKey = Boolean(eventInitDict.ctrlKey);
		this.#detail = Number(eventInitDict.detail ?? 0);
		this.#isComposing = Boolean(eventInitDict.isComposing);
		this.#key = String(eventInitDict.key ?? "");
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
	
	get code(): string {
		return this.#code;
	}
	
	get ctrlKey(): boolean {
		return this.#ctrlKey;
	}
	
	get isComposing(): boolean {
		return this.#isComposing;
	}
	
	get key(): string {
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

