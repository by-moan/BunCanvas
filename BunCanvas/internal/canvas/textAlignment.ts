import { CanvasTextAlignmentEnum } from "../native/enums";


export const CanvasTextAlignment = {
    "left": CanvasTextAlignmentEnum.Left,
    "right": CanvasTextAlignmentEnum.Right,
    "center": CanvasTextAlignmentEnum.Center,
    "start": CanvasTextAlignmentEnum.Start,
    "end": CanvasTextAlignmentEnum.End,
}

export type CanvasTextAlignmentArg = "left" | "right" | "center" | "start" | "end"