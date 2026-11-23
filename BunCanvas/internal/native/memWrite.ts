// Write memory on a Uint32Array
export function writePtrU64(buf: Uint32Array, index: number, ptr: number): number {
	buf[index]     = ptr >>> 0;                 // Low 32 bits (stays in register)
	buf[index + 1] = (ptr / 0x100000000) >>> 0; // High 32 bits (divides by 2^32, truncates fraction)
	return index + 2;
}