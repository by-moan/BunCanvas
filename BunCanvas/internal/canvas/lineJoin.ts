import { CanvasLineJoinEnum } from "../native/enums";

export const CanvasLineJoin = {
    "round": CanvasLineJoinEnum.kRound_Join,
    "bevel": CanvasLineJoinEnum.kBevel_Join,
    "miter": CanvasLineJoinEnum.kMiter_Join,
}

export type CanvasLineJoinArg = "round" | "bevel" | "miter"