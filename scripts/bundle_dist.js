// build.js
import { spawn } from "bun";

// 1. Build JS with Bun
const buildResult = await Bun.build({
  entrypoints: ['./BunCanvas/index.ts'],
  outdir: './publish/core',
  target: 'bun',
  format: 'esm',
  external: ['@buncanvas/*']
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

const filePath = "./publish/core/index.js";

let content = await Bun.file(filePath).text();



console.log("Build and type generation completed successfully!");

console.log("Applying bugfix patch for bundling issue.");

content = content.replace(/var\s+__require\s*=\s*import\.meta\.require;?\r?\n?/, "");

// 2. Replace the first occurrence of __require with require
content = content.replace("__require", "require");

await Bun.write(filePath, content);

console.log("Successfully patched index.js require call!");