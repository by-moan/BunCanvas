type ImageDataOptions = {
    colorSpace?: "srgb" | "display-p3";
    pixelFormat?: "rgba-unorm8";
};

export class ImageData {
    readonly width: number;
    readonly height: number;
    readonly data: Uint8ClampedArray;
    readonly colorSpace: "srgb" | "display-p3";

    // Overload 1: ImageData(width, height, settings?)
    constructor(width: number, height: number, settings?: ImageDataOptions);
    // Overload 2: ImageData(data, width, height?, settings?)
    constructor(dataArray: Uint8ClampedArray, width: number, height?: number, settings?: ImageDataOptions);

    // Implementation signature
    constructor(
        arg1: number | Uint8ClampedArray,
        arg2: number,
        arg3?: number | ImageDataOptions,
        arg4?: ImageDataOptions
    ) {
        let data: Uint8ClampedArray;
        let width: number;
        let height: number;
        let settings: ImageDataOptions | undefined;

        if (arg1 instanceof Uint8ClampedArray) {
            // Constructor signature: (dataArray, width, height?, settings?)
            data = arg1;
            width = arg2;

            if (typeof arg3 === "number") {
                height = arg3;
                settings = arg4;
            } else {
                settings = arg3;
                height = (data.length / 4) / width;
            }

            if (data.length === 0 || data.length % 4 !== 0) {
                throw new SyntaxError("The input data array length must be a non-zero multiple of 4.");
            }

            if (width <= 0 || !Number.isInteger(width)) {
                throw new SyntaxError("Width must be a positive integer.");
            }

            if (height <= 0 || !Number.isInteger(height)) {
                throw new SyntaxError("Height must be a positive integer or derived from array length.");
            }

            if (data.length !== width * height * 4) {
                throw new SyntaxError("The input data array length does not match width * height * 4.");
            }
        } else if (typeof arg1 === "number" && typeof arg2 === "number") {
            // Constructor signature: (width, height, settings?)
            width = arg1;
            height = arg2;
            settings = arg3 as ImageDataOptions | undefined;

            if (width <= 0 || !Number.isInteger(width) || height <= 0 || !Number.isInteger(height)) {
                throw new SyntaxError("Width and height must be positive integers.");
            }

            data = new Uint8ClampedArray(width * height * 4);
        } else {
            throw new TypeError("Failed to construct 'ImageData': Invalid arguments provided.");
        }

        this.width = width;
        this.height = height;
        this.data = data;
        this.colorSpace = settings?.colorSpace ?? "srgb";
    }
}