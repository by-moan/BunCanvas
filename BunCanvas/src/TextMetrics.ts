import { INTERNAL_TOKEN, throwIllegalConstructor } from "../internal/ConstructEnforcer";

export class TextMetrics {
        readonly width : number;
        readonly actualBoundingBoxLeft : number;
        readonly actualBoundingBoxRight : number;
        readonly actualBoundingBoxAscent : number;
        readonly actualBoundingBoxDescent : number;
        readonly fontBoundingBoxAscent : number;
        readonly fontBoundingBoxDescent : number;
        readonly emHeightAscent : number;
        readonly emHeightDescent : number;
        readonly alphabeticBaseline : number;
        readonly hangingBaseline : number;
        readonly ideographicBaseline : number;

        constructor(
            token: {},
            width : number,
            actualBoundingBoxLeft : number,
            actualBoundingBoxRight : number,
            actualBoundingBoxAscent : number,
            actualBoundingBoxDescent : number,
            fontBoundingBoxAscent : number,
            fontBoundingBoxDescent : number,
            emHeightAscent : number,
            emHeightDescent : number,
            alphabeticBaseline : number,
            hangingBaseline : number,
            ideographicBaseline : number
        ){
            if (token !== INTERNAL_TOKEN) throwIllegalConstructor(this)
            this.width = width
            this.actualBoundingBoxLeft = actualBoundingBoxLeft
            this.actualBoundingBoxRight = actualBoundingBoxRight
            this.actualBoundingBoxAscent = actualBoundingBoxAscent
            this.actualBoundingBoxDescent = actualBoundingBoxDescent
            this.fontBoundingBoxAscent = fontBoundingBoxAscent
            this.fontBoundingBoxDescent = fontBoundingBoxDescent
            this.emHeightAscent = emHeightAscent
            this.emHeightDescent = emHeightDescent
            this.alphabeticBaseline = alphabeticBaseline
            this.hangingBaseline = hangingBaseline
            this.ideographicBaseline = ideographicBaseline
        }
    }