import { Canvas } from "./BunCanvas/Canvas";
import Window from "./BunCanvas/Window";

const window = new Window(800,600)

const canvas = new Canvas()
const ctx = canvas.getContext()

let x = 0;
let y = 0;

let loop = setInterval(()=>{
    ctx.clearRect(0,0,1000,1000)
    ctx.fillStyle = "#ff00ff"
    ctx.fillRect(x,y,10,10)
    x+=1
    y+=1
},10)