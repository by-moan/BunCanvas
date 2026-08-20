import { Window, requestAnimationFrame } from "./BunCanvas/Window.ts";
import { Canvas } from "./BunCanvas/Canvas.ts";
import { Image } from "./BunCanvas/Image.ts";
import { DOMMatrix } from "./BunCanvas/DOMMatrix.ts";
import { Audio } from "./BunCanvas/Audio.ts";

const window = new Window(800,600, "spiiin", {vsync:false});
// Create and append canvas
const canvas = new Canvas(1000,1000)
const ctx = canvas.getContext("2d", {desynchronized:true})
window.append(canvas);
let x = 0;
let y = 0;
let count = 0;

while (count <= 0x111111) {
    const color = count.toString(16).padStart(6, "0");
	console.log(color)
    ctx.fillStyle = "#" + color;
    ctx.fillRect(x, y, 1, 1);

    y++;

    if (y > 1020) {
        y = 0;
        x++;
    }

    count++;
}