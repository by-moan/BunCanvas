import {Window, Canvas, requestAnimationFrame} from "./BunCanvas/BunCanvas";


const canvas = new Canvas(600,500)
const ctx = canvas.getContext()

const window = new Window(800,600)


window.append(canvas)

let x = 0;
let y = 0;

let count = 0;

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0
},1000)


window.addEventListener("resize", (evt)=>{
    console.log(`Width: ${window.innerWidth} Height: ${window.innerHeight}`)
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
})
window.addEventListener("mousemove", (evt)=>{
    // console.log(`X: ${evt.clientX} Y: ${evt.clientY} mX: ${evt.movementX} mY: ${evt.movementY}`)
    x = evt.clientX
    y = evt.clientY
})

function loop() {
    // console.log("aaaa")
    count++;
    ctx.clearRect(0,0,window.innerWidth, window.innerHeight)
    ctx.fillStyle = "tan"
    ctx.strokeStyle = "#413b72"
    ctx.lineWidth = 2

    ctx.fillRect(x,y,10,10)

    ctx.beginPath();
    ctx.moveTo(20, 20);
    ctx.lineTo(100, 20);
    ctx.arcTo(150, 20, 150, 70, 50);
    ctx.lineTo(150, 120);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(20, 20);
    ctx.bezierCurveTo(20, 100, 200, 100, 200, 20);
    // Draw the path
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(100, 75, 50, 0, 2 * Math.PI);
    ctx.stroke();
    requestAnimationFrame(loop)
}

loop()