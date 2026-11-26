
if (!process.versions.bun) {
    throw new Error('BunCanvas only supports Bun! Other runtimes like Node or Deno are unsupported.')
}

export { Image } from "./src/Image";

export { FontFace } from "./src/FontFace";

export { TextMetrics } from "./src/TextMetrics";

export { MouseEvent } from "./src/MouseEvent";
export { WheelEvent } from "./src/WheelEvent";
export { KeyboardEvent } from "./src/KeyboardEvent";

export { CanvasGradient } from "./src/Canvas/canvasGradient";
export { CanvasRenderingContext2D } from "./src/Canvas/RenderingContexts/RenderingContext2D";

export { Canvas } from "./src/Canvas";

export { DOMMatrix } from "./dom/DOMMatrix";
export { Path2D } from "./src/Path2D";

export { Window, requestAnimationFrame } from "./src/Window";

export { Audio } from "./src/Audio";