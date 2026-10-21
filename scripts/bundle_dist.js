// build.js
import { spawn } from "bun";

// 1. Build JS with Bun
const buildResult = await Bun.build({
  entrypoints: ['./BunCanvas/index.ts'],
  outdir: './dist',
  target: 'bun',
  format: 'esm',
  minify: true
});

if (!buildResult.success) {
  console.error("JS build failed:", buildResult.logs);
  process.exit(1);
}

// 2. Emit .d.ts files via tsc
const tscProcess = spawn(["bunx", "tsc", "--emitDeclarationOnly"]);
const exitCode = await tscProcess.exited;

if (exitCode !== 0) {
  console.error("Type generation failed.");
  process.exit(1);
}

console.log("Build and type generation completed successfully!");