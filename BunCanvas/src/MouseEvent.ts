export interface MouseEventInit extends EventInit {
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
	
	// MouseEventInit properties
	button?: number;
	buttons?: number;
	clientX?: number;
	clientY?: number;
	movementX?: number;
	movementY?: number;
	relatedTarget?: EventTarget | null;
	screenX?: number;
	screenY?: number;
}

export class MouseEvent extends Event {
	// Private backing fields for runtime read-only enforcement
	#altKey: boolean;
	#button: number;
	#buttons: number;
	#clientX: number;
	#clientY: number;
	#ctrlKey: boolean;
	#detail: number;
	#metaKey: boolean;
	#movementX: number;
	#movementY: number;
	#relatedTarget: EventTarget | null;
	#screenX: number;
	#screenY: number;
	#shiftKey: boolean;
	#view: Window | null;
	#modifiers: Map<string, boolean>;
	
	constructor(type: string, eventInitDict: MouseEventInit = {}) {
		// Pass standard EventInit flags (bubbles, cancelable, composed) to Bun's Event
		super(type, {
			bubbles: eventInitDict.bubbles ?? false,
			cancelable: eventInitDict.cancelable ?? false,
			composed: eventInitDict.composed ?? false,
		});
		
		this.#altKey = Boolean(eventInitDict.altKey);
		this.#button = Number(eventInitDict.button ?? 0);
		this.#buttons = Number(eventInitDict.buttons ?? 0);
		this.#clientX = Number(eventInitDict.clientX ?? 0);
		this.#clientY = Number(eventInitDict.clientY ?? 0);
		this.#ctrlKey = Boolean(eventInitDict.ctrlKey);
		this.#detail = Number(eventInitDict.detail ?? 0);
		this.#metaKey = Boolean(eventInitDict.metaKey);
		this.#movementX = Number(eventInitDict.movementX ?? 0);
		this.#movementY = Number(eventInitDict.movementY ?? 0);
		this.#relatedTarget = eventInitDict.relatedTarget ?? null;
		this.#screenX = Number(eventInitDict.screenX ?? 0);
		this.#screenY = Number(eventInitDict.screenY ?? 0);
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
	
	// Standard MouseEvent readonly getters
	get altKey(): boolean {
		return this.#altKey;
	}
	
	get button(): number {
		return this.#button;
	}
	
	get buttons(): number {
		return this.#buttons;
	}
	
	get clientX(): number {
		return this.#clientX;
	}
	
	get clientY(): number {
		return this.#clientY;
	}
	
	get ctrlKey(): boolean {
		return this.#ctrlKey;
	}
	
	get metaKey(): boolean {
		return this.#metaKey;
	}
	
	get movementX(): number {
		return this.#movementX;
	}
	
	get movementY(): number {
		return this.#movementY;
	}
	
	get relatedTarget(): EventTarget | null {
		return this.#relatedTarget;
	}
	
	get screenX(): number {
		return this.#screenX;
	}
	
	get screenY(): number {
		return this.#screenY;
	}
	
	get shiftKey(): boolean {
		return this.#shiftKey;
	}
	
	// Standard coordinate aliases & UIEvent properties
	get x(): number {
		return this.#clientX;
	}
	
	get y(): number {
		return this.#clientY;
	}
	
	get pageX(): number {
		return this.#clientX + (this.#view?.scrollX ?? 0);
	}
	
	get pageY(): number {
		return this.#clientY + (this.#view?.scrollY ?? 0);
	}
	
	get offsetX(): number {
		return this.#clientX;
	}
	
	get offsetY(): number {
		return this.#clientY;
	}
	
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