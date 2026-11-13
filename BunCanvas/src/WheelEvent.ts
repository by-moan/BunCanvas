import { MouseEvent, type MouseEventInit } from "./MouseEvent";

export interface WheelEventInit extends MouseEventInit {
	deltaX?: number;
	deltaY?: number;
	deltaZ?: number;
	deltaMode?: number;
}

export class WheelEvent extends MouseEvent {
	// Standard W3C WheelEvent deltaMode constants
	static readonly DOM_DELTA_PIXEL = 0x00;
	static readonly DOM_DELTA_LINE = 0x01;
	static readonly DOM_DELTA_PAGE = 0x02;
	
	// Private backing fields for runtime read-only enforcement
	#deltaX: number;
	#deltaY: number;
	#deltaZ: number;
	#deltaMode: number;
	
	constructor(type: string, eventInitDict: WheelEventInit = {}) {
		// Pass all MouseEventInit and EventInit options up to MouseEvent -> Event
		super(type, eventInitDict);
		
		this.#deltaX = Number(eventInitDict.deltaX ?? 0);
		this.#deltaY = Number(eventInitDict.deltaY ?? 0);
		this.#deltaZ = Number(eventInitDict.deltaZ ?? 0);
		this.#deltaMode = Number(eventInitDict.deltaMode ?? WheelEvent.DOM_DELTA_PIXEL);
	}
	
	// Standard WheelEvent readonly getters
	get deltaX(): number {
		return this.#deltaX;
	}
	
	get deltaY(): number {
		return this.#deltaY;
	}
	
	get deltaZ(): number {
		return this.#deltaZ;
	}
	
	get deltaMode(): number {
		return this.#deltaMode;
	}
}