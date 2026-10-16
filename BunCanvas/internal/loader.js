import { suffix } from "bun:ffi";
import { readFileSync } from "node:fs";
import { join } from "node:path";
// Helper to resolve dynamically when running directly with Bun
export function getNativePath() {
  const archMap = { x64: "x64", arm: "aarch64", arm64: "aarch64" };
  const targetArch = archMap[process.arch] || process.arch;
  return require(`../bin/BunCanvas.${targetArch}.${suffix}`);
}