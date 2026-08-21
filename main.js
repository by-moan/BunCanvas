import { Window, requestAnimationFrame } from "./BunCanvas/Window.ts";
import { Canvas } from "./BunCanvas/Canvas.ts";
import { Image } from "./BunCanvas/Image.ts";
import { DOMMatrix } from "./BunCanvas/DOMMatrix.ts";
import { Audio } from "./BunCanvas/Audio.ts";

const window = new Window(800,600, "spiiin", {vsync:false});
// Create and append canvas
const canvas = new Canvas(1000,1000)
const ctx = canvas.getContext("2d", {desynchronized:true})
window.append(canvas);

{
    const img2 = new Image()
    img2.src = "https://yt3.ggpht.com/WgdNsGixrvdVpuGZ9lIdadaLX5qx7Rfl2ewVuPtg-QPFW1uBk2z-fiH5SlSLoQ9UP_DCAO2tbg=s48-c-k-c0x00ffffff-no-rj"
}
const img = new Image()
img.src = "https://media.discordapp.net/attachments/1379212171506094171/1540150588573220904/magesuwu_2.png?ex=6a88e882&is=6a879702&hm=21d3ae06ae5468359f9c39eb69b1a1f1661a0fcf5b7c94e559e039aba628bb68&animated=true&width=1711&height=1403"
img.onload = ()=>{
    console.log(img.src)
    console.time("draw")
    ctx.drawImage(img,0,0,1711,1403)
    console.timeEnd("draw")
}