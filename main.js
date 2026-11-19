import { Window, Canvas } from "./BunCanvas";

const window = new Window(800, 600, "Benchmark", { vsync: false, maxFramerate: 0 });
const canvas = new Canvas(800, 600);
window.append(canvas);
const ctx = canvas.getContext("2d", { desynchronized: true });

const COLORS = ["red", "green", "blue", "yellow", "cyan", "magenta"];

function runBatchPass(drawCalls) {
    const t0 = Bun.nanoseconds();

    for (let i = 0; i < drawCalls; i++) {
        // Alternate state + draw calls to simulate real-world canvas workloads
        ctx.fillStyle = COLORS[i % COLORS.length];
        ctx.fillRect((i * 7) % 780, (i * 13) % 580, 20, 20);
    }

    const tRecordEnd = Bun.nanoseconds();

    // Force synchronous flush so we measure the actual C++ replay cost,
    // rather than deferring it past the timer via setImmediate

    const tFlushEnd = Bun.nanoseconds();

    return {
        recordMs: (tRecordEnd - t0) / 1e6,
        flushMs: (tFlushEnd - tRecordEnd) / 1e6,
        totalMs: (tFlushEnd - t0) / 1e6,
    };
}

// 1. Warmup (lets JSC FTL JIT optimize the loops and populates encodeCString cache)
for (let i = 0; i < 200; i++) {
    runBatchPass(500);
}


// 2. Benchmark across draw-call counts
for (const count of [100, 1_000, 5_000, 20_000]) {
    const iterations = 500;
    let sumRecord = 0;
    let sumFlush = 0;
    let sumTotal = 0;

    for (let i = 0; i < iterations; i++) {
        const res = runBatchPass(count);
        sumRecord += res.recordMs;
        sumFlush += res.flushMs;
        sumTotal += res.totalMs;
    }

    console.log(
        `[${count.toString().padStart(5)} draws] ` +
        `Record: ${(sumRecord / iterations).toFixed(3)} ms | ` +
        `Flush: ${(sumFlush / iterations).toFixed(3)} ms | ` +
        `Total: ${(sumTotal / iterations).toFixed(3)} ms`
    );
}

ctx.clearRect(0,0,window.innerWidth, window.innerHeight)