import { Window, requestAnimationFrame } from "./BunCanvas/Window.ts";
import { Canvas } from "./BunCanvas/Canvas.ts";
import { Image } from "./BunCanvas/Image";
import { DOMMatrix } from "./BunCanvas/DOMMatrix.ts";

const window = new Window(800,600, "spiiin", {vsync:false});
// Create and append canvas
const canvas = new Canvas(800,600)
window.append(canvas);

window.onresize = (evt)=>{
	canvas.width = window.innerWidth
	canvas.height = window.innerHeight
}

const ctx = canvas.getContext('2d');

// Cube vertices
const vertices = [
  [-100, -100, -100], [100, -100, -100], [100, 100, -100], [-100, 100, -100],
  [-100, -100, 100], [100, -100, 100], [100, 100, 100], [-100, 100, 100]
];

// Faces defined by 4 vertex indices
const faces = [
  { verts: [0, 1, 2, 3], color: '#ff6b6b', name: 'back' },
  { verts: [4, 7, 6, 5], color: '#4ecdc4', name: 'front' },
  { verts: [0, 4, 5, 1], color: '#45b7d1', name: 'bottom' },
  { verts: [2, 6, 7, 3], color: '#96ceb4', name: 'top' },
  { verts: [0, 3, 7, 4], color: '#ffeaa7', name: 'left' },
  { verts: [1, 5, 6, 2], color: '#dfe6e9', name: 'right' }
];

// Create offscreen canvases with patterns for each face
const faceCanvases = {};
faces.forEach((face, i) => {
	const img = new Image()
	img.src = "./Kawkaw_battle_idle.gif"
	img.width = 200
	img.height = 140
  const fc = new Canvas(200,200);
  const fctx = fc.getContext('2d');
  
  // Fill background
  fctx.fillStyle = face.color;
  fctx.fillRect(0, 0, 200, 200);
  
  // Draw a pattern
  fctx.strokeStyle = 'rgba(0,0,0,0.3)';
  fctx.lineWidth = 3;
  for (let j = 0; j < 200; j += 40) {
    fctx.beginPath();
    fctx.moveTo(0, j);
    fctx.lineTo(200, j);
    fctx.stroke();
    fctx.beginPath();
    fctx.moveTo(j, 0);
    fctx.lineTo(j, 200);
    fctx.stroke();
  }
  
  // Draw face label
  fctx.fillStyle = 'rgba(0,0,0,0.7)';
  fctx.font = 'bold 30px Arial';
  fctx.textAlign = 'center';
  fctx.textBaseline = 'middle';
  fctx.fillText(face.name.toUpperCase(), 100, 100);
  
  // Draw border
  fctx.strokeStyle = 'rgba(255,255,255,0.5)';
  fctx.lineWidth = 4;
  fctx.strokeRect(0, 0, 200, 200);
  img.onload = ()=>{
    fctx.drawImage(img, 0, 30, 200, 140);
  }
  
  faceCanvases[i] = fc;
});

let angleX = 0;
let angleY = 0;

function rotateY(point, angle) {
  const cos = Math.cos(angle);
  const sin = Math.sin(angle);
  const x = point[0] * cos + point[2] * sin;
  const z = -point[0] * sin + point[2] * cos;
  return [x, point[1], z];
}

function rotateX(point, angle) {
  const cos = Math.cos(angle);
  const sin = Math.sin(angle);
  const y = point[1] * cos - point[2] * sin;
  const z = point[1] * sin + point[2] * cos;
  return [point[0], y, z];
}

function project(point) {
  const distance = 500;
  const scale = distance / (distance + point[2]);
  return [
    point[0] * scale + canvas.width / 2,
    point[1] * scale + canvas.height / 2,
    point[2]
  ];
}

// Draw a textured quadrilateral by splitting into two triangles
// Uses transformation (setTransform) to map image corners to projected corners
function drawTexturedQuad(img, pts) {
  const [p0, p1, p2, p3] = pts;
  
  // Triangle 1: p0, p1, p3  (maps to top-left, top-right, bottom-left of image)
  drawTriangle(img, p0, p1, p3, 0, 0, 200, 0, 0, 200);
  // Triangle 2: p1, p2, p3  (maps to top-right, bottom-right, bottom-left of image)
  drawTriangle(img, p1, p2, p3, 200, 0, 200, 200, 0, 200);
}

function drawTriangle(img, p0, p1, p2, u0, v0, u1, v1, u2, v2) {
  ctx.save();
  
  // Calculate affine transformation matrix
  // Maps: (u0,v0)->p0, (u1,v1)->p1, (u2,v2)->p2
  const x0 = p0[0], y0 = p0[1];
  const x1 = p1[0], y1 = p1[1];
  const x2 = p2[0], y2 = p2[1];
  
  const denom = (u0 - u1) * (v0 - v2) - (u0 - u2) * (v0 - v1);
  if (Math.abs(denom) < 0.001) return;
  
  // Matrix elements for ctx.setTransform(a, b, c, d, e, f)
  const a = ((x0 - x1) * (v0 - v2) - (x0 - x2) * (v0 - v1)) / denom;
  const c = ((x0 - x2) * (u0 - u1) - (x0 - x1) * (u0 - u2)) / denom;
  const b = ((y0 - y1) * (v0 - v2) - (y0 - y2) * (v0 - v1)) / denom;
  const d = ((y0 - y2) * (u0 - u1) - (y0 - y1) * (u0 - u2)) / denom;
  const e = x0 - a * u0 - c * v0;
  const f = y0 - b * u0 - d * v0;
  ctx.setTransform(a, b, c, d, e, f);
  
  // Clip to triangle and draw
  ctx.beginPath();
  ctx.moveTo(u0, v0);
  ctx.lineTo(u1, v1);
  ctx.lineTo(u2, v2);
  ctx.closePath();
  ctx.clip();
  ctx.drawImage(img, 0, 0,200,200);
  
  ctx.restore();
}

function draw() {
  ctx.fillStyle = '#303e44';
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  // Transform vertices
  const transformedVerts = vertices.map(v => {
    let r = rotateY(v, angleY);
    r = rotateX(r, angleX);
    return r;
  });
  
  // Project vertices and calculate face depths
  const projectedFaces = faces.map((face, i) => {
    const pts = face.verts.map(vi => project(transformedVerts[vi]));
    const z = pts.reduce((sum, p) => sum + p[2], 0) / 4;
    return { face, pts, z, index: i };
  });
  
  // Sort by depth (painter's algorithm - back faces first)
  projectedFaces.sort((a, b) => a.z - b.z);
  
  // Draw faces
  projectedFaces.forEach(({ face, pts, index }) => {
    // Backface culling - skip faces pointing away
    const [p0, p1, p2] = pts;
    const cross = (p1[0] - p0[0]) * (p2[1] - p0[1]) - (p1[1] - p0[1]) * (p2[0] - p0[0]);
    if (cross < 0) return;
    
    drawTexturedQuad(faceCanvases[index], pts);
    
    // Draw wireframe edge
    ctx.strokeStyle = 'rgba(255,255,255,0)';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(pts[0][0], pts[0][1]);
    for (let i = 1; i < pts.length; i++) {
      ctx.lineTo(pts[i][0], pts[i][1]);
    }
    ctx.closePath();
    ctx.stroke();
  });
  
  angleY += 0.02;
  angleX += 0.01;
  
  requestAnimationFrame(draw);
}

draw();