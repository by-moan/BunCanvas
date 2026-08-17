import { Canvas } from "./BunCanvas/Canvas";
import { requestAnimationFrame, Window } from "./BunCanvas/Window";
import { Image } from "./BunCanvas/Image";
import { Path2D } from "./BunCanvas/Path2D";
const WIDTH = 1000;
const HEIGHT = 600;
if (!globalThis.window) globalThis.window = new Window(WIDTH, HEIGHT);
const canvas = new Canvas(WIDTH, HEIGHT);
const ctx = canvas.getContext("2d", {alpha:false});

window.append(canvas);

ctx.fillStyle = "#fff"
ctx.fillRect(0,0,WIDTH,HEIGHT)

ctx.fillStyle = "#000"
ctx.transform(1, 0.2, 0.8, 1, 0, 0);
ctx.fillRect(0, 0, 100, 100);