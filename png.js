import { deflateSync } from "node:zlib";

const PNG_SIGNATURE = Uint8Array.from([
    137, 80, 78, 71, 13, 10, 26, 10
]);

//
// CRC32
//

const crcTable = new Uint32Array(256);

for (let i = 0; i < 256; i++) {
    let c = i;
    for (let j = 0; j < 8; j++) {
        c = (c & 1)
            ? (0xEDB88320 ^ (c >>> 1))
            : (c >>> 1);
    }
    crcTable[i] = c >>> 0;
}

function crc32(data) {
    let crc = 0xFFFFFFFF;

    for (let i = 0; i < data.length; i++) {
        crc = crcTable[(crc ^ data[i]) & 0xFF] ^ (crc >>> 8);
    }

    return (crc ^ 0xFFFFFFFF) >>> 0;
}

//
// PNG chunk
//

function makeChunk(type, data) {
    const chunk = new Uint8Array(12 + data.length);
    const view = new DataView(chunk.buffer);

    view.setUint32(0, data.length);

    chunk[4] = type.charCodeAt(0);
    chunk[5] = type.charCodeAt(1);
    chunk[6] = type.charCodeAt(2);
    chunk[7] = type.charCodeAt(3);

    chunk.set(data, 8);

    const crcInput = new Uint8Array(4 + data.length);
    crcInput.set(chunk.subarray(4, 8));
    crcInput.set(data, 4);

    view.setUint32(8 + data.length, crc32(crcInput));

    return chunk;
}

export async function writePNG(path, width, height, rgba) {

    if (rgba.length !== width * height * 4)
        throw new Error("Invalid RGBA buffer.");

    //
    // Build scanlines.
    // Every row starts with filter byte 0.
    //

    const stride = width * 4;
    const raw = new Uint8Array(height * (stride + 1));

    let src = 0;
    let dst = 0;

    for (let y = 0; y < height; y++) {

        raw[dst++] = 0; // Filter None

        raw.set(
            rgba.subarray(src, src + stride),
            dst
        );

        src += stride;
        dst += stride;
    }

    const compressed = deflateSync(raw);

    //
    // IHDR
    //

    const ihdr = new Uint8Array(13);
    const ihdrView = new DataView(ihdr.buffer);

    ihdrView.setUint32(0, width);
    ihdrView.setUint32(4, height);

    ihdr[8] = 8; // bit depth
    ihdr[9] = 6; // RGBA
    ihdr[10] = 0; // compression
    ihdr[11] = 0; // filter
    ihdr[12] = 0; // interlace

    const chunks = [
        makeChunk("IHDR", ihdr),
        makeChunk("IDAT", compressed),
        makeChunk("IEND", new Uint8Array(0))
    ];

    let total =
        PNG_SIGNATURE.length +
        chunks.reduce((a, c) => a + c.length, 0);

    const png = new Uint8Array(total);

    let offset = 0;

    png.set(PNG_SIGNATURE, offset);
    offset += PNG_SIGNATURE.length;

    for (const chunk of chunks) {
        png.set(chunk, offset);
        offset += chunk.length;
    }

    await Bun.write(path, png);
}