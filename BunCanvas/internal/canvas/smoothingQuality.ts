import { CanvasSmoothingQualityEnum } from "../native/enums";

export const CanvasSmoothingQuality = {
    "low": CanvasSmoothingQualityEnum.Low,
    "medium": CanvasSmoothingQualityEnum.Medium,
    "high": CanvasSmoothingQualityEnum.High,
};

export type CanvasSmoothingQualityArg = "low" | "medium" | "high"