import { SkBlendMode } from "../native/enums";

export const CanvasBlendMode = {
    "source-over": SkBlendMode.kSrcOver,
    "source-in": SkBlendMode.kSrcIn,
    "source-out": SkBlendMode.kSrcOut,
    "source-atop": SkBlendMode.kSrcATop,
    "destination-over": SkBlendMode.kDstOver,
    "destination-in": SkBlendMode.kDstIn,
    "destination-out": SkBlendMode.kDstOut,
    "destination-atop": SkBlendMode.kDstATop,
    "lighter": SkBlendMode.kLighten,
    "copy": SkBlendMode.kSrc,
    "xor": SkBlendMode.kXor,
    "multiply": SkBlendMode.kMultiply,
    "screen": SkBlendMode.kScreen,
    "overlay": SkBlendMode.kOverlay,
    "darken": SkBlendMode.kDarken,
    "lighten": SkBlendMode.kLighten,
    "color-dodge": SkBlendMode.kColorDodge,
    "color-burn": SkBlendMode.kColorBurn,
    "hard-light": SkBlendMode.kHardLight,
    "soft-light": SkBlendMode.kSoftLight,
    "difference": SkBlendMode.kDifference,
    "exclusion": SkBlendMode.kExclusion,
    "hue": SkBlendMode.kHue,
    "saturation": SkBlendMode.kSaturation,
    "color": SkBlendMode.kColor,
    "luminosity": SkBlendMode.kLuminosity,
}

export type CanvasBlendModeArg = "source-over" | "source-in" | "source-out" | "source-atop" | "destination-over" | "destination-in" | "destination-out" | "destination-atop" | "lighter" | "copy" | "xor" | "multiply" | "screen" | "overlay" | "darken" | "lighten" | "color-dodge" | "color-burn" | "hard-light" | "soft-light" | "difference" | "exclusion" | "hue" | "saturation" | "color" | "luminosity"