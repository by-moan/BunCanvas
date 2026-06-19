import { Canvas } from "./BunCanvas/Canvas";
import Window, {requestAnimationFrame} from "./BunCanvas/Window";

const window = new Window(800,600)

const canvas = new Canvas()
const ctx = canvas.getContext()

let x = 0;
let y = 0;

let count = 0;

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0
},1000)


function loop() {
    // console.log("aaaa")
    count++;
    ctx.fillStyle = "#ff00ff"
    ctx.fillRect(x,y,10,10)
    x+=1
    y+=1

    requestAnimationFrame(loop)
}

loop()