import {Canvas} from "BunCanvas";



const canvas = new Canvas(600,500, {gpu:false})
const ctx = canvas.getContext("2d")

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
    ctx.fillRect(0,0,50,50)
    ctx.globalCompositeOperation = "source-over"

let dat = ctx.getImageData(20,20,50,50);
    console.log(dat)