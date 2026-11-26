import { Constructor } from "../Mixin";
import { setHandler } from "./setEventHandler";

export interface IMouseEventTarget {
    onmousemove: ((evt: MouseEvent) => any) | null;
    onmousedown: ((evt: MouseEvent) => any) | null;
    onmouseup: ((evt: MouseEvent) => any) | null;
    onclick: ((evt: MouseEvent) => any) | null;
    oncontextmenu: ((evt: MouseEvent) => any) | null;
    onwheel: ((evt: WheelEvent) => any) | null;
}

export type MouseEventListenerNames = "mousemove" | "mousedown" | "mouseup" | "click" | "contextmenu" | "wheel"

export function MouseEventMixin<TBase extends Constructor<EventTarget>>(
    Base: TBase
): Constructor<IMouseEventTarget> & TBase {
    return class MouseEventTarget extends Base implements IMouseEventTarget {
        #mousemoveHandler: ((evt: MouseEvent) => any) | null = null;
        #mousedownHandler: ((evt: MouseEvent) => any) | null = null;
        #mouseupHandler: ((evt: MouseEvent) => any) | null = null;
        #clickHandler: ((evt: MouseEvent) => any) | null = null;
        #contextMenuHandler: ((evt: MouseEvent) => any) | null = null;
        #wheelHandler: ((evt: WheelEvent) => any) | null = null;

        get onmousemove() { return this.#mousemoveHandler; }
        set onmousemove(fn: ((evt: MouseEvent) => any) | null) { this.#mousemoveHandler = setHandler(this, 'mousemove', this.#mousemoveHandler, fn); }

        get onmousedown() { return this.#mousedownHandler; }
        set onmousedown(fn: ((evt: MouseEvent) => any) | null) { this.#mousedownHandler = setHandler(this, 'mousedown', this.#mousedownHandler, fn); }

        get onmouseup() { return this.#mouseupHandler; }
        set onmouseup(fn: ((evt: MouseEvent) => any) | null) { this.#mouseupHandler = setHandler(this, 'mouseup', this.#mouseupHandler, fn); }

        get onclick() { return this.#clickHandler; }
        set onclick(fn: ((evt: MouseEvent) => any) | null) { this.#clickHandler = setHandler(this, 'click', this.#clickHandler, fn); }

        get oncontextmenu() { return this.#contextMenuHandler; }
        set oncontextmenu(fn: ((evt: MouseEvent) => any) | null) { this.#contextMenuHandler = setHandler(this, 'contextmenu', this.#contextMenuHandler, fn); }

        get onwheel() { return this.#wheelHandler; }
        set onwheel(fn: ((evt: WheelEvent) => any) | null) { this.#wheelHandler = setHandler(this, 'wheel', this.#wheelHandler, fn); }
    };
}

// Still export a standalone class if you instantiate MouseEventTarget directly elsewhere
export class MouseEventTarget extends MouseEventMixin(EventTarget) {}