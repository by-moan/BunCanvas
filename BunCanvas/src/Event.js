export class Event {
    type = null
    target = null
    timeStamp = performance.now()

    constructor(type, target) {
        this.type = type;
        this.target = target;
    }
}
