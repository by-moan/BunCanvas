import { Window, requestAnimationFrame, Canvas, Image, DOMMatrix, Audio } from "./BunCanvas";


const window = new Window(800,600, "spiiin", {vsync:false, maxFramerate:165});
// Create and append canvas
const canvas = new Canvas(800,600)
window.append(canvas);

window.onresize = (evt)=>{
	canvas.width = window.innerWidth
	canvas.height = window.innerHeight
}

const ctx = canvas.getContext('2d');

const mPos = {x:0,y:0}

window.onmousemove = (evt)=>{
	mPos.x = evt.clientX
	mPos.y = evt.clientY
}

let snd = new Audio("./Kawkaw_voiceclip_happy_1.wav")

window.onmousedown = (evt)=>{
	console.log(evt)
	if (evt.buttons == 1) {
		snd.play()
	}
	if (evt.buttons == 2) {
		snd.stop()
	}
}

function loop(){
	ctx.clearRect(0,0,window.innerWidth,window.innerHeight)
	ctx.fillStyle = "red"
	ctx.fillRect(mPos.x-50,mPos.y-50,100,100)
	// ctx.drawImage(img, 100,100, 100,100,200,200,100,100)

	requestAnimationFrame(loop)
}

loop()