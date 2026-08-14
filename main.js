import { Canvas } from "./BunCanvas/Canvas";
import { requestAnimationFrame, Window } from "./BunCanvas/Window";
import { Image } from "./BunCanvas/Image";
const WIDTH = 1000;
const HEIGHT = 600;
const window = new Window(WIDTH, HEIGHT);
const canvas = new Canvas(WIDTH, HEIGHT);
const ctx = canvas.getContext("2d");

window.append(canvas);

ctx.font = "30px arial"
ctx.fillStyle = "#fff"

function loop(){
    ctx.clearRect(0,0,WIDTH,HEIGHT)
    ctx.filter = "drop-shadow(20px 20px 20px red)"
    ctx.shadowBlur = 20
    ctx.shadowOffsetX = -30
    ctx.shadowOffsetY = 30
    ctx.shadowColor = "#00f"
    ctx.fillText("the quick brown fox jumps over the lazy dog", 100,100);
    console.log(ctx.measureText("the quick brown fox jumps over the lazy dog").width)
    ctx.filter = "none"
    requestAnimationFrame(loop)
}

loop();