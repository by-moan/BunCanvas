import { CanvasTextBaseLineEnum } from "../native/enums";

export const CanvasTextBaseline = {
    "top": CanvasTextBaseLineEnum.Top,
    "hanging": CanvasTextBaseLineEnum.Hanging,
    "middle": CanvasTextBaseLineEnum.Middle,
    "alphabetic": CanvasTextBaseLineEnum.Alphabetic,
    "ideographic": CanvasTextBaseLineEnum.Ideographic,
    "bottom": CanvasTextBaseLineEnum.Bottom,
}

export type CanvasTextBaselineArg = "top" | "hanging" | "middle" | "alphabetic" | "ideographic" | "bottom"