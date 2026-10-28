import { Window, requestAnimationFrame, Canvas, Image, DOMMatrix, Audio, Path2D } from "./BunCanvas";

const window = new Window(800,600, "spiiin", {vsync:false, maxFramerate:165});
// Create and append canvas
const canvas = new Canvas(800,600)
window.append(canvas);

window.onresize = (evt)=>{
	canvas.width = window.innerWidth
	canvas.height = window.innerHeight
}

const ctx = canvas.getContext('2d', {desynchronized:true});

// Forward native canvas events to canvas/window callback properties
if (canvas) {
	const getPos = (e) => {
		const rect = canvas.getBoundingClientRect();
		const clientX = e.touches ? e.touches[0].clientX : e.clientX;
		const clientY = e.touches ? e.touches[0].clientY : e.clientY;
		return {
			x: (clientX - rect.left) * (canvas.width / rect.width),
			y: (clientY - rect.top) * (canvas.height / rect.height)
		};
	};
	
	window.addEventListener('mousedown', (e) => {
		const pos = getPos(e);
		const evt = { x: pos.x, y: pos.y, button: e.button, type: 'mousedown' };
		if (canvas.onmousedown) canvas.onmousedown(evt);
	});
	
	window.addEventListener('mousemove', (e) => {
		const pos = getPos(e);
		const evt = { x: pos.x, y: pos.y, type: 'mousemove' };
		if (canvas.onmousemove) canvas.onmousemove(evt);
	});
	
	window.addEventListener('mouseup', (e) => {
		const evt = { type: 'mouseup' };
		if (canvas.onmouseup) canvas.onmouseup(evt);
	});
	if (typeof document !== 'undefined') {
		document.addEventListener('mouseup', (e) => {
			const evt = { type: 'mouseup' };
			if (canvas.onmouseup) canvas.onmouseup(evt);
		});
	}
	
	window.addEventListener('touchstart', (e) => {
		e.preventDefault();
		const pos = getPos(e);
		const evt = { x: pos.x, y: pos.y, type: 'touchstart' };
		if (canvas.ontouchstart) canvas.ontouchstart(evt);
	}, { passive: false });
	
	window.addEventListener('touchmove', (e) => {
		e.preventDefault();
		const pos = getPos(e);
		const evt = { x: pos.x, y: pos.y, type: 'touchmove' };
		if (canvas.ontouchmove) canvas.ontouchmove(evt);
	}, { passive: false });
	
	window.addEventListener('touchend', (e) => {
		const evt = { type: 'touchend' };
		if (canvas.ontouchend) canvas.ontouchend(evt);
	});
}

// Physics Configuration Settings
const PHYSICS = {
	gravity: 0.35,        // Downward force
	restitution: 0.82,    // Bounciness coefficient [0..1]
	friction: 0.998,      // Air resistance
	floorFriction: 0.97,  // Sliding friction on walls/floor
	subSteps: 4,          // Iteration solver substeps for stability
	spawnRateMs: 35,      // Delay between continuous ball spawns (ms)
	defaultRadius: 10     // Required radius = 10px
};

// Ball Class
class Ball {
	static radius = 10
	constructor(x, y, radius = 10, vx = 0, vy = 0) {
		this.x = x;
		this.y = y;
		this.radius = radius;
		this.vx = vx;
		this.vy = vy;
		this.mass = Math.PI * radius * radius;
		this.r = (Math.random() * 360) | 0;
		this.g = (Math.random() * 360) | 0;
		this.b = (Math.random() * 360) | 0;
		this.color = `rgb(${this.r},${this.g},${this.b})`;
	}
	
	update(dt) {
		// Apply Gravity & Air Drag
		this.vy += PHYSICS.gravity * dt;
		this.vx *= PHYSICS.friction;
		this.vy *= PHYSICS.friction;
		
		// Update Coordinates
		this.x += this.vx * dt;
		this.y += this.vy * dt;
	}

	static BallPath = new Path2D()
	static BallShinePath = new Path2D()
	
	draw(ctx) {
		ctx.save();
		ctx.translate(this.x, this.y);
		
		// Glow & Color
		ctx.fillStyle = this.color;
		ctx.fill(Ball.BallPath);
		
		// Highlight arc for depth
		ctx.beginPath();
		ctx.fillStyle = 'rgba(255, 255, 255, 0.4)';
		ctx.fill(Ball.BallShinePath);
		
		ctx.restore();
	}
}

Ball.BallPath.arc(0,0, Ball.radius, 0, Math.PI * 2)
Ball.BallShinePath.arc(-Ball.radius*.3,-Ball.radius*.3, Ball.radius * 0.35, 0, Math.PI * 2)

// State variables
const balls = [];
let isMouseDown = false;
let mouseX = window.innerWidth / 2;
let mouseY = window.innerHeight / 2;
let lastSpawnTime = 0;

// Statistics
let frameCount = 0;
let fps = 0;
let lastFpsUpdate = performance.now();

// Mouse & Touch Event Handlers
canvas.onmousedown = (e) => {
	isMouseDown = true;
	mouseX = e.x;
	mouseY = e.y;
	spawnBall(mouseX, mouseY);
	lastSpawnTime = performance.now();
};

canvas.onmousemove = (e) => {
	mouseX = e.x;
	mouseY = e.y;
};

canvas.onmouseup = () => {
	isMouseDown = false;
};

canvas.ontouchstart = (e) => {
	isMouseDown = true;
	mouseX = e.x;
	mouseY = e.y;
	spawnBall(mouseX, mouseY);
	lastSpawnTime = performance.now();
};

canvas.ontouchmove = (e) => {
	mouseX = e.x;
	mouseY = e.y;
};

canvas.ontouchend = () => {
	isMouseDown = false;
};

// Spawn a single 10px ball with slight random scatter velocity
function spawnBall(x, y) {
	const scatterX = (Math.random() - 0.5) * 4;
	const scatterY = (Math.random() - 0.5) * 4;
	balls.push(new Ball(x, y, PHYSICS.defaultRadius, scatterX, scatterY));
}

// Boundary Collision Resolution
function resolveBoundary(ball) {
	const w = window.innerWidth;
	const h = window.innerHeight;
	
	// Floor
	if (ball.y + ball.radius > h) {
		ball.y = h - ball.radius;
		ball.vy *= -PHYSICS.restitution;
		ball.vx *= PHYSICS.floorFriction;
	}
	// Ceiling
	else if (ball.y - ball.radius < 0) {
		ball.y = ball.radius;
		ball.vy *= -PHYSICS.restitution;
	}
	
	// Left Wall
	if (ball.x - ball.radius < 0) {
		ball.x = ball.radius;
		ball.vx *= -PHYSICS.restitution;
	}
	// Right Wall
	else if (ball.x + ball.radius > w) {
		ball.x = w - ball.radius;
		ball.vx *= -PHYSICS.restitution;
	}
}

// Ball-to-Ball Elastic Collisions
function resolveCollisions() {
	const len = balls.length;
	for (let i = 0; i < len; i++) {
		for (let j = i + 1; j < len; j++) {
			const b1 = balls[i];
			const b2 = balls[j];
			
			const dx = b2.x - b1.x;
			const dy = b2.y - b1.y;
			const distSq = dx * dx + dy * dy;
			const minDist = b1.radius + b2.radius;
			
			if (distSq < minDist * minDist && distSq > 0) {
				const dist = Math.sqrt(distSq);
				const nx = dx / dist;
				const ny = dy / dist;
				
				// Separate overlapping balls
				const overlap = 0.5 * (minDist - dist);
				b1.x -= nx * overlap;
				b1.y -= ny * overlap;
				b2.x += nx * overlap;
				b2.y += ny * overlap;
				
				// Elastic impulse calculation
				const kx = b1.vx - b2.vx;
				const ky = b1.vy - b2.vy;
				const p = 2 * (nx * kx + ny * ky) / (b1.mass + b2.mass);
				
				if (p > 0) {
					const impulse = p * PHYSICS.restitution;
					b1.vx -= impulse * b2.mass * nx;
					b1.vy -= impulse * b2.mass * ny;
					b2.vx += impulse * b1.mass * nx;
					b2.vy += impulse * b1.mass * ny;
				}
			}
		}
	}
}

// Overlay rendering
function drawCanvasHUD() {
	ctx.save();
	
	// Header info box
	ctx.fillStyle = 'rgba(15, 23, 42, 0.7)';
	ctx.strokeStyle = 'rgba(255, 255, 255, 0.1)';
	ctx.lineWidth = 1;
	ctx.beginPath();
	ctx.roundRect(16, 16, 210, 80, 8);
	ctx.fill();
	ctx.stroke();
	
	// Text
	ctx.fillStyle = '#6366f1';
	ctx.font = 'bold 13px system-ui, sans-serif';
	ctx.fillText(`WINDOW: asd`, 28, 38);
	
	ctx.fillStyle = '#10b981';
	ctx.font = '12px monospace';
	ctx.fillText(`FPS: ${fps}`, 28, 58);
	
	ctx.fillStyle = '#e2e8f0';
	ctx.fillText(`Objects: ${balls.length}`, 110, 58);
	
	ctx.fillStyle = '#94a3b8';
	ctx.font = '11px system-ui, sans-serif';
	ctx.fillText('Hold click to spawn 10px balls', 28, 78);
	
	// Spawning Indicator under Cursor
	if (isMouseDown) {
		ctx.beginPath();
		ctx.arc(mouseX, mouseY, PHYSICS.defaultRadius + 4, 0, Math.PI * 2);
		ctx.strokeStyle = 'rgba(99, 102, 241, 0.6)';
		ctx.lineWidth = 2;
		ctx.setLineDash([4, 4]);
		ctx.stroke();
	}
	
	ctx.restore();
}

// Main Loop
let lastFrameTime = performance.now();

function step(now) {
	const dt = Math.min((now - lastFrameTime) / 16.667, 2.0);
	lastFrameTime = now;
	
	// Handle Continuous Spawning while Mouse Down
	if (isMouseDown && (now - lastSpawnTime >= PHYSICS.spawnRateMs)) {
		spawnBall(mouseX, mouseY);
		lastSpawnTime = now;
	}
	
	// Sub-step physics solver for stability
	const subDt = dt / PHYSICS.subSteps;
	for (let s = 0; s < PHYSICS.subSteps; s++) {
		for (let i = 0; i < balls.length; i++) {
			balls[i].update(subDt);
			resolveBoundary(balls[i]);
		}
		resolveCollisions();
	}
	
	// Canvas Background clear with subtle trail fade
	ctx.fillStyle = 'rgba(9, 13, 22, 0.35)';
	ctx.fillRect(0, 0, window.innerWidth, window.innerHeight);
	
	// Render all ball objects
	for (let i = 0; i < balls.length; i++) {
		balls[i].draw(ctx);
	}
	
	// Render Canvas HUD
	drawCanvasHUD();
	
	// FPS Counter
	frameCount++;
	if (now - lastFpsUpdate >= 500) {
		fps = Math.round((frameCount * 1000) / (now - lastFpsUpdate));
		frameCount = 0;
		lastFpsUpdate = now;
	}
	requestAnimationFrame(step);
}

// Seed initial balls
for (let i = 0; i < 20; i++) {
	const rx = 100 + Math.random() * (window.innerWidth - 200);
	const ry = 50 + Math.random() * (window.innerHeight / 2);
	balls.push(new Ball(rx, ry, PHYSICS.defaultRadius, (Math.random() - 0.5) * 6, (Math.random() - 0.5) * 4));
}

// Start Animation Frame Loop
requestAnimationFrame(step);