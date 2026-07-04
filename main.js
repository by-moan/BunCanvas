import {Window, Canvas, requestAnimationFrame, Image} from "./BunCanvas/BunCanvas";



const window = new Window(800,600)

const canvas = new Canvas(window.innerWidth,window.innerHeight)
const ctx = canvas.getContext("2d")

const img = new Image();

img.onload = (evt)=>{
    console.log("image loaded!", evt.timeStamp)
}
img.onerror = (evt)=>{
    console.log("error loading image!", evt.timeStamp)
}
img.src = "./face.png";

window.append(canvas)

let x = 0;
let y = 0;

let count = 0;

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0
},1000)


window.addEventListener("resize", (evt)=>{
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
})
window.addEventListener("mousemove", (evt)=>{
    x = evt.clientX
    y = evt.clientY
})

window.addEventListener("keydown", (evt)=>{
    console.log("key pressed!", evt.code, evt.key)
})

function loop() {
    count++;
    ctx.clearRect(0,0,window.innerWidth, window.innerHeight)
    // ctx.fillStyle = "cornflowerblue"
    // ctx.fillRect(x-25,y-25,50,50)
    // ctx.strokeStyle = "#413b72"
    // ctx.lineWidth = 2

    // ctx.beginPath();
    // ctx.moveTo(20, 20);
    // ctx.lineTo(100, 20);
    // ctx.arcTo(150, 20, 150, 70, 50);
    // ctx.lineTo(150, 120);
    // ctx.stroke();

    // ctx.beginPath();
    // ctx.moveTo(20, 20);
    // ctx.bezierCurveTo(20, 100, 200, 100, 200, 20);
    // ctx.stroke();

    // ctx.beginPath();
    // ctx.arc(100, 75, 50, 0, 2 * Math.PI);
    // ctx.stroke();

    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    ctx.drawImage(img, window.innerWidth*Math.random(),window.innerHeight*Math.random(),150,150);
    console.log("loop")

    // ctx.globalCompositeOperation = "screen"
    ctx.drawImage(img, x-75,y-75,150,150);
    ctx.drawImage(img, x-175,y-75,150,150);
    ctx.drawImage(img, x-275,y-75,150,150);
    ctx.drawImage(img, x-375,y-75,150,150);
    // ctx.globalCompositeOperation = "source-over"

    // let data = ctx.getImageData(10,10,200,200);
    // ctx.putImageData(data,30,200)
    requestAnimationFrame(loop)
}

loop()