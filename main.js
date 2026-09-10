import { Window, requestAnimationFrame, Canvas, Image, DOMMatrix, Audio } from "./BunCanvas/index";


const window = new Window(800,600, "spiiin", {vsync:false});
// Create and append canvas
const canvas = new Canvas(800,600)
window.append(canvas);

window.onresize = (evt)=>{
	canvas.width = window.innerWidth
	canvas.height = window.innerHeight
}

const ctx = canvas.getContext('2d');

const img = new Image()
img.src = "https://cdn.discordapp.com/attachments/1496392566545846282/1547125417746366484/52E2294F-07C9-41F4-9BD8-8E412CA3C49A.png?ex=6aa24853&is=6aa0f6d3&hm=aa31a38944ed2ea678f4eaf32422d4fab73676c1c9fedbb7ced86cde05526324&animated=true"

img.onload =()=>{
	console.log("load!")
}
img.onerror =()=>{
	console.log("error!")
}
function loop(){
	ctx.clearRect(0,0,window.innerWidth,window.innerHeight)
	ctx.fillStyle = "red"
	// ctx.fillRect(100,100,500,700)
	ctx.drawImage(img, 100,100, 100,100,200,200,100,100)

	requestAnimationFrame(loop)
}

loop()