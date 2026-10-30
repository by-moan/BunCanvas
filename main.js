import { Canvas, Window, requestAnimationFrame } from './BunCanvas'

const window = new Window(800,600)
const canvas = new Canvas(window.innerWidth, window.innerHeight)
const ctx = canvas.getContext('2d')

window.onresize = ()=>{
  canvas.width = window.innerWidth;
  canvas.height = window.innerHeight;
}

//Make it visible to the window.
window.append(canvas)

const coords = {
  x:0,
  y:0
}

window.onmousemove = (evt)=>{
  coords.x = evt.clientX
  coords.y = evt.clientY
}

window.onkeyup = (evt)=>{
	console.log(evt)
}

function loop(){
  ctx.clearRect(0,0,window.innerWidth, window.innerHeight)
  ctx.fillStyle = 'red'
  ctx.fillRect(coords.x-50,coords.y-50,100,100)
  requestAnimationFrame(loop)
}

requestAnimationFrame(loop)