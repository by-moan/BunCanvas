import {Window, Canvas, requestAnimationFrame} from "./BunCanvas/BunCanvas";

const window = new Window(800,600)

const canvas = new Canvas(600,500)
const ctx = canvas.getContext()

window.append(canvas)

let x = 0;
let y = 0;

let count = 0;

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0
},1000)


window.onresize = (evt)=>{
    console.log(`Width: ${window.innerWidth} Height: ${window.innerHeight}`)
}
window.onmousemove = (evt)=>{
    console.log(`X: ${evt.clientX} Y: ${evt.clientY} mX: ${evt.movementX} mY: ${evt.movementY}`)
    x = evt.clientX
    y = evt.clientY
}

function loop() {
    // console.log("aaaa")
    // ctx.clearRect(200,200,1000,1000)
    count++;
    ctx.fillStyle = "#ff00ff"
    ctx.fillRect(x,y,10,10)

    requestAnimationFrame(loop)
}

loop()