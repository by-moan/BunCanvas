import {Window, Canvas, requestAnimationFrame, Image} from "./BunCanvas/BunCanvas.ts";
import { FontFace } from "./BunCanvas/FontFace.js";
import { writePNG } from "./png.js";


const window = new Window(800,600)

const canvas = new Canvas(200,200)
const ctx = canvas.getContext("2d")


// const canvas2 = new Canvas(100,100)
// const ctx2 = canvas2.getContext("2d")

// ctx2.fillStyle = "#ff0000";
// ctx2.fillRect(0,0,100,100)

// const img = new Image();

// img.onload = (evt)=>{
//     console.log("image loaded!", evt.timeStamp)
// }
// img.onerror = (evt)=>{
//     console.log("error loading image!", evt.timeStamp)
// }
// img.src = "./face.png";

window.append(canvas)

const addedFont = new FontFace("YuyuShort","./YuyuShort-Regular.ttf");

await addedFont.load();

window.fonts.add(addedFont)

// ctx.font = "60px regular YuyuShort"

// let x = 0;
// let y = 0;

let count = 0;

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0
},1000)

window.addEventListener("resize", (evt)=>{
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    // ctx.font = "60px regular YuyuShort"
})

// window.addEventListener("mousemove", (evt)=>{
//     x = evt.clientX
//     y = evt.clientY
// })

// window.addEventListener("keydown", (evt)=>{
//     console.log("key pressed!", evt.code, evt.key)
// })

export default function drawText(){
    let count = 140,
        rng = {intBetween:(min,max)=>~~(Math.random() * (max-min +1)) + min}

    for (let i=0; i<count; i++){
        let x = rng.intBetween(0, 400),
            y = rng.intBetween(0, 400),
            weight = rng.intBetween(1,9) * 100,
            size = rng.intBetween(8, 200),
            char = String.fromCharCode(rng.intBetween(33, 93)),
            color = rng.intBetween(0, 255).toString(16)
        ctx.fillStyle = '#'+color+color+color
        ctx.font = `${weight} ${size}px YuyuShort`
        ctx.fillText(char, x, y)
    }

    return ctx.getImageData(0,0,1024,768)
}

const ITERATIONS = 200;

let loop = ()=>{
    count++;
    ctx.clearRect(0,0,window.innerWidth, window.innerHeight);
    drawText()
    requestAnimationFrame(loop);
}

loop();