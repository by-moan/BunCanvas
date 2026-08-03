import { Window, requestAnimationFrame } from "./BunCanvas/Window.ts";
import { Canvas } from "./BunCanvas/Canvas.ts";
import { FontFace } from "./BunCanvas/FontFace.js";
import { Image } from "./BunCanvas/Image";


const window = new Window(800,600,"AEOU")

const canvas = new Canvas(1000,1000)
const ctx = canvas.getContext("2d", {alpha: false, desynchronized: true})

const img = new Image()
img.src = "./face.png"
window.append(canvas)
let grad = ctx.createConicGradient(0,250,250)
grad.addColorStop(0,"red")
grad.addColorStop(0.5,"lime")
grad.addColorStop(1,"blue")

window.addEventListener("resize", ()=>{
    canvas.width = window.innerWidth
    canvas.height = window.innerHeight
})

ctx.fillStyle = grad
function loop(){
    ctx.clearRect(0,0,window.innerWidth, window.innerHeight)
    // ctx.filter = "blur(10px)"
    ctx.fillRect(100,100,300,300)
    // requestAnimationFrame(loop)
}
setInterval(loop,0)