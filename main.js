import { Window, requestAnimationFrame, Canvas,Image,Audio,DOMMatrix } from "./BunCanvas/index.ts";


const window = new Window(800,600, "spiiin", {vsync:true});
// Create and append canvas
const canvas = new Canvas(800,600)
window.append(canvas);

window.onresize = (evt)=>{
    canvas.width = window.innerWidth
    canvas.height = window.innerHeight
}
const ctx = canvas.getContext('2d');

window.onkeydown = (evt)=>{
    console.log(evt.key)
}

function loop(){
    ctx.clearRect(0,0,canvas.width, canvas.height)
    ctx.fillStyle = "red"
    ctx.fillRect(100,100,100,100)
    requestAnimationFrame(loop)
}
loop()