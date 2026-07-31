import { Window, requestAnimationFrame } from "./BunCanvas/Window.ts";
import { Canvas } from "./BunCanvas/Canvas.ts";
import { FontFace } from "./BunCanvas/FontFace.js";
import { Image } from "./BunCanvas/Image";
import { cc, toArrayBuffer } from "bun:ffi"
import test from "./test.ignore.c" with { type: "text" };

const path = "/tmp/test.c"
await Bun.write(path,test)
const custom = cc({
  source:path,
  symbols: {
    create: {
      args: [],
      returns: "ptr",
    },
    getPosFromObject: {
      args: ["ptr"],
      returns: "ptr",
    },
    getVelFromObject: {
      args: ["ptr"],
      returns: "ptr",
    },
    insert: {
      args: ["ptr"],
      returns: "void",
    },
    UpdateAll: {
      args: ["double"],
      returns: "void",
    },
  },
});
await Bun.file(path).delete();

const window = new Window(800,600, "App", {vsync:true, maxFramerate : 165})

const canvas = new Canvas(window.innerWidth, window.innerHeight)
const ctx = canvas.getContext("2d")

window.onresize = ()=>{
    canvas.width = window.innerWidth
    canvas.height = window.innerHeight
}

let clicking = false;

class Entity {
    ptr = custom.symbols.create();
    pos = new Float64Array(toArrayBuffer(custom.symbols.getPosFromObject(this.ptr),0, 2 * 8))
    vel = new Float64Array(toArrayBuffer(custom.symbols.getVelFromObject(this.ptr),0, 2 * 8))

    constructor(){
        this.vel[0] = 2
    }

    draw(){
        ctx.fillStyle = "#f00"
        ctx.fillRect(this.pos[0]-15, this.pos[1]-15, 30,30)
        ctx.fillStyle = "#000"
    }
}
let entities = []
function insert(obj){
    custom.symbols.insert(obj.ptr)
    entities.push(obj)
}

function UpdateAll(n) {
    custom.symbols.UpdateAll(n);
}

window.onclick = ()=>{
    insert(new Entity())
}
let x = 0;
let y = 0;
window.onmousemove = (evt)=>{
    x = evt.clientX
    y = evt.clientY
}

window.append(canvas)

let count = 0

setInterval(()=>{
    console.log(`${count}fps`)
    count = 0;
},1000)



let loop = ()=>{
    count++
    ctx.clearRect(0,0,window.innerWidth, window.innerHeight)
    UpdateAll(0.1)
    for (const item of entities) {
        item.draw();
    }
    requestAnimationFrame(loop)
}
loop()