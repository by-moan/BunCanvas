export class ImageData {
    width : number;
    height : number;
    data : Uint8Array;
    constructor(width : number,height : number,data : Uint8Array){
        this.width = width;
        this.height = height;
        this.data = data;
    }
}