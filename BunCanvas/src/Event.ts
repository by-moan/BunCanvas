export class Event {
    type : any = null
    target : any = null
    timeStamp = performance.now()

    constructor(type?: string, target?: string) {
        this.type = type;
        this.target = target;
    }
}
