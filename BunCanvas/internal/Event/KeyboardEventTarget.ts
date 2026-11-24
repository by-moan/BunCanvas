import { Constructor } from "../Mixin";
import { setHandler } from "./setEventHandler";

export interface IKeyboardEventTarget {
    onkeydown: ((evt: KeyboardEvent) => any) | null;
    onkeyup: ((evt: KeyboardEvent) => any) | null;
}

export type KeyboardEventListenerNames = "keydown" | "keyup"

export function KeyboardEventMixin<TBase extends Constructor<EventTarget>>(
    Base: TBase
): Constructor<IKeyboardEventTarget> & TBase {
    return class KeyboardEventTarget extends Base implements IKeyboardEventTarget {
        #keydownHandler: ((evt: KeyboardEvent) => any) | null = null;
        #keyupHandler: ((evt: KeyboardEvent) => any) | null = null;

        get onkeydown() { return this.#keydownHandler; }
        set onkeydown(fn: ((evt: KeyboardEvent) => any) | null) { this.#keydownHandler = setHandler(this, 'keydown', this.#keydownHandler, fn); }

        get onkeyup() { return this.#keyupHandler; }
        set onkeyup(fn: ((evt: KeyboardEvent) => any) | null) { this.#keyupHandler = setHandler(this, 'keyup', this.#keyupHandler, fn); }
    };
}

export class KeyboardEventTarget extends KeyboardEventMixin(EventTarget) {}