console.time("init")
const tStart = performance.now();
import { Window, requestAnimationFrame } from "./BunCanvas/Window.ts";
import { Canvas } from "./BunCanvas/BunCanvas.ts";
import { FontFace } from "./BunCanvas/FontFace.js";
import { Image } from "./BunCanvas/Image";
import { writePNG } from "./png.js";



const window = new Window(800,600)

const canvas = new Canvas(200,200)
const ctx = canvas.getContext("2d")


// const canvas2 = new Canvas(100,100)
// const ctx2 = canvas2.getContext("2d")

// ctx2.fillStyle = "#ff0000";
// ctx2.fillRect(0,0,100,100)

const img = new Image();

img.onload = (evt)=>{
    console.log("image loaded!", evt.timeStamp)
}
img.onerror = (evt)=>{
    console.log("error loading image!", evt.timeStamp)
}
img.src = "./bunlogo.png";

window.append(canvas)

const addedFont = new FontFace("YuyuShort","./YuyuShort-Regular.ttf");

await addedFont.load();

window.fonts.add(addedFont)

// ctx.font = "60px regular YuyuShort"

let x = 0;
let y = 0;
let s = 0;
let count = 0;

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0
},1000)

setInterval(()=>{
    s+=0.05
},5)

window.addEventListener("resize", (evt)=>{
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    // ctx.font = "60px regular YuyuShort"
})

window.addEventListener("mousemove", (evt)=>{
    x = evt.clientX
    y = evt.clientY
})

const grad = ctx.createLinearGradient(20,0,220,0);
grad.addColorStop(0, "green");
// grad.addColorStop(-0.5, "cyan");
grad.addColorStop(0.7, "blue");
grad.addColorStop(1, "green");

let loop = ()=>{
    count++;
    // ctx.fillStyle = "#101010"
    ctx.clearRect(0,0,window.innerWidth, window.innerHeight);
    ctx.fillStyle = "#101010"
    ctx.fillRect(0,0,window.innerWidth/2, window.innerHeight/2);
    ctx.fillStyle = grad;
    ctx.fillRect(20, 20, 200, 100);
    ctx.strokeStyle = "#0f0"
    ctx.fillStyle = "#f0a"
    ctx.shadowColor = "#d99b9b"
    ctx.lineWidth = 10;
    ctx.shadowBlur = (Math.sin(s)+10)*10;
    ctx.drawImage(img,200,200,396,347)
    ctx.strokeRect(x-150,y-150,300,300)
    ctx.shadowBlur = 0;
    ctx.fillRect(x-50,y-50,100,100)
    requestAnimationFrame(loop);
}
console.log(performance.now()-tStart)
console.timeEnd("init")

loop();