import { Canvas } from "./BunCanvas/Canvas";
import Window, {requestAnimationFrame} from "./BunCanvas/Window";

const window = new Window(800,600)

const canvas = new Canvas(500,500)
console.log("canvas created")
const ctx = canvas.getContext()
console.log("context created")

window.append(canvas)

let x = 0;
let y = 0;

let count = 0;

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0
},1000)


function loop() {
    // console.log("aaaa")
    // ctx.clearRect(200,200,1000,1000)
    count++;
    ctx.fillStyle = "#ff00ff"
    ctx.fillRect(x,y,10,10)
    x+=0.1
    y+=0.1

    requestAnimationFrame(loop)
}

loop()