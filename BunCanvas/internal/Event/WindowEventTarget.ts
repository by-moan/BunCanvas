import { Mixin } from "../Mixin";
import { KeyboardEventMixin, KeyboardEventListenerNames } from "./KeyboardEventTarget";
import { MouseEventMixin, MouseEventListenerNames } from "./MouseEventTarget";
import { setHandler } from "./setEventHandler";

export interface IWindowEventTarget {
    onmousemove: ((evt: MouseEvent) => any) | null;
    onmousedown: ((evt: MouseEvent) => any) | null;
    onmouseup: ((evt: MouseEvent) => any) | null;
    onclick: ((evt: MouseEvent) => any) | null;
    oncontextmenu: ((evt: MouseEvent) => any) | null;
    onwheel: ((evt: WheelEvent) => any) | null;

    onkeydown: ((evt: KeyboardEvent) => any) | null;
    onkeyup: ((evt: KeyboardEvent) => any) | null;

    onresize: ((evt: Event) => any) | null;
    onfocus: ((evt: FocusEvent) => any) | null;
    onblur: ((evt: FocusEvent) => any) | null;
}

export type WindowEventListenerNames = "resize" | "focus" | "blur" | KeyboardEventListenerNames | MouseEventListenerNames

export class WindowEventTarget extends Mixin(EventTarget, MouseEventMixin, KeyboardEventMixin) implements EventTarget, IWindowEventTarget {
    #resizeHandler: ((evt: Event) => any) | null = null;
    #focusHandler: ((evt: FocusEvent) => any) | null = null;
    #blurHandler: ((evt: FocusEvent) => any) | null = null;

    get onresize() { return this.#resizeHandler; }
    set onresize(fn: ((evt: Event) => any) | null) { this.#resizeHandler = setHandler(this, 'resize', this.#resizeHandler, fn); }

    get onfocus() { return this.#focusHandler; }
    set onfocus(fn: ((evt: FocusEvent) => any) | null) { this.#focusHandler = setHandler(this, 'focus', this.#focusHandler, fn); }

    get onblur() { return this.#blurHandler; }
    set onblur(fn: ((evt: FocusEvent) => any) | null) { this.#blurHandler = setHandler(this, 'blur', this.#blurHandler, fn); }
}