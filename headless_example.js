import {Window, Canvas, requestAnimationFrame, Image} from "./BunCanvas/BunCanvas";



const canvas = new Canvas(600,500)
const ctx = canvas.getContext("2d")

const img = new Image();

img.onload = (evt)=>{
    console.log("image loaded!", evt.timeStamp)
}
img.onerror = (evt)=>{
    console.log("error loading image!", evt.timeStamp)
}
img.src = "./face.png";

ctx.fillStyle = "cornflowerblue"
    ctx.strokeStyle = "#413b72"
    ctx.lineWidth = 2

    ctx.beginPath();
    ctx.moveTo(20, 20);
    ctx.lineTo(100, 20);
    ctx.arcTo(150, 20, 150, 70, 50);
    ctx.lineTo(150, 120);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(20, 20);
    ctx.bezierCurveTo(20, 100, 200, 100, 200, 20);
    ctx.stroke();

    ctx.beginPath();
    ctx.arc(100, 75, 50, 0, 2 * Math.PI);
    ctx.stroke();

    

    ctx.globalCompositeOperation = "screen"
    ctx.drawImage(img, 40,40,150,150);
    // ctx.fillRect(x-25,y-25,50,50)
    ctx.globalCompositeOperation = "source-over"

    let data = ctx.getImageData(10,10,10,10);
    ctx.putImageData(data,30,200)


setTimeout(()=>{
    let dat = ctx.getImageData(20,20,50,50);
    console.log(dat)
},2000)