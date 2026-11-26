import { createHash } from "node:crypto";
import { basename, join } from "node:path";
import { tmpdir } from "node:os";
import { Pointer, ptr } from "bun:ffi";
import { lib, encoder } from "../internal/symbols.js";
import { ptrs } from "../internal/pointers.js";
import { setHandler } from "../internal/Event/setEventHandler.js";

const fRegistry = new FinalizationRegistry((pointer: bigint | Pointer | null) => {
	lib.symbols.image_destroy(pointer);
});

const URL_REGEX = /^https?:\/\/(?:www\.)?[^\s/$.?#].[^\s]*$/i;

function computeTempPath(url: string): string {
	const hash = createHash("sha256").update(url).digest("hex").slice(0, 16);
	const { pathname } = new URL(url);
	const filename = basename(pathname) || "index";
	return join(tmpdir(), `${hash}-${filename}`);
}

async function fetchToTemp(url: string, targetPath = computeTempPath(url)): Promise<string> {
	const response = await fetch(url);
	if (!response.ok || !response.body) {
		throw new Error(`HTTP ${response.status}: ${url}`);
	}
	await Bun.write(targetPath, await response.arrayBuffer());
	return targetPath;
}

export class Image extends EventTarget {
	#loadHandler: ((evt: Event) => unknown) | null = null;
	#errorHandler: ((evt: Event) => unknown) | null = null;

	#src = "";
	#naturalWidth = 0;
	#naturalHeight = 0;
	#width: number | null = null;
	#height: number | null = null;
	#complete = true; // true initially when src is empty/unset
	#loadId = 0;
	#pendingDecode: Promise<void> | null = null;

	constructor(width?: number, height?: number) {
		super();
		const pointer = lib.symbols.image_create();
		ptrs.set(this, pointer);
		fRegistry.register(this, pointer);

		if (width !== undefined) this.width = width;
		if (height !== undefined) this.height = height;
	}

	get onload() {
		return this.#loadHandler;
	}
	set onload(fn: ((evt: Event) => unknown) | null) {
		this.#loadHandler = setHandler(this, "load", this.#loadHandler, fn);
	}

	get onerror() {
		return this.#errorHandler;
	}
	set onerror(fn: ((evt: Event) => unknown) | null) {
		this.#errorHandler = setHandler(this, "error", this.#errorHandler, fn);
	}

	get src(): string {
		return this.#src;
	}

	set src(value: string) {
		this.#src = value;
		const currentLoadId = ++this.#loadId;

		// setting src to empty string resets state without firing error
		if (!value) {
			this.#naturalWidth = 0;
			this.#naturalHeight = 0;
			this.#complete = true;
			this.#pendingDecode = null;
			return;
		}

		this.#complete = false;
		const isRemote = URL_REGEX.test(value);
		const localPath = isRemote ? computeTempPath(value) : value;
		const nativePtr = ptrs.get(this);

		lib.symbols.image_set_src(nativePtr, encoder.encode(`${localPath}\0`), isRemote);

		// Avoid capturing `this` directly in async closures so WeakRef can collect if abandoned
		const weakSelf = new WeakRef(this);

		const loadTask = (async () => {
			try {
				if (isRemote) {
					await fetchToTemp(value, localPath);
				} else {
					// Defer local load to next microtask/macrotask so event listeners attached
					// synchronously right after `img.src = ...` are guaranteed to fire.
					await new Promise<void>((resolve) => setTimeout(resolve, 0));
				}

				const self = weakSelf.deref();
				if (!self || self.#loadId !== currentLoadId) return;

				self.#finalizeLoad(nativePtr);
			} catch {
				const self = weakSelf.deref();
				if (!self || self.#loadId !== currentLoadId) return;

				self.#failLoad();
			}
		})();

		this.#pendingDecode = loadTask.then(() => {
			if (!this.#complete || (this.#naturalWidth === 0 && this.#naturalHeight === 0)) {
				throw new DOMException("The source image cannot be decoded.", "EncodingError");
			}
		});
		// Prevent unhandled rejection warnings if decode() isn't called
		this.#pendingDecode.catch(() => {});
	}

	// Shared native FFI load & event dispatch logic.
	#finalizeLoad(nativePtr: bigint | Pointer | null): void {
		const dimensions = new Int32Array(2);
		const success = Boolean(lib.symbols.image_load(nativePtr, ptr(dimensions)));

		if (success) {
			this.#naturalWidth = dimensions[0];
			this.#naturalHeight = dimensions[1];
			this.#complete = true;
			this.dispatchEvent(new Event("load"));
		} else {
			this.#failLoad();
		}
	}

	#failLoad(): void {
		this.#naturalWidth = 0;
		this.#naturalHeight = 0;
		this.#complete = true; // Spec: complete becomes true even when image fails to load
		this.dispatchEvent(new Event("error"));
	}

	// --- HTMLImageElement Spec Properties & Methods ---

	get complete(): boolean {
		return this.#complete;
	}

	get naturalWidth(): number {
		return this.#naturalWidth;
	}

	get naturalHeight(): number {
		return this.#naturalHeight;
	}

	get width(): number {
		return this.#width ?? this.#naturalWidth;
	}

	set width(value: number) {
		this.#width = Math.max(0, Math.floor(Number(value) || 0));
	}

	get height(): number {
		return this.#height ?? this.#naturalHeight;
	}

	set height(value: number) {
		this.#height = Math.max(0, Math.floor(Number(value) || 0));
	}

	decode(): Promise<void> {
		if (!this.#src) {
			return Promise.reject(new DOMException("Invalid image source.", "EncodingError"));
		}
		if (this.#pendingDecode) {
			return this.#pendingDecode;
		}
		return this.#naturalWidth > 0 || this.#naturalHeight > 0
			? Promise.resolve()
			: Promise.reject(new DOMException("The source image cannot be decoded.", "EncodingError"));
	}
}