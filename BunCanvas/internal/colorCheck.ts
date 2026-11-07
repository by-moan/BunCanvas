const NAMED_COLORS = new Set([
    "aliceblue", "antiquewhite", "aqua", "aquamarine", "azure", "beige", "bisque", "black",
    "blanchedalmond", "blue", "blueviolet", "brown", "burlywood", "cadetblue", "chartreuse",
    "chocolate", "coral", "cornflowerblue", "cornsilk", "crimson", "cyan", "darkblue",
    "darkcyan", "darkgoldenrod", "darkgray", "darkgreen", "darkgrey", "darkkhaki",
    "darkmagenta", "darkolivegreen", "darkorange", "darkorchid", "darkred", "darksalmon",
    "darkseagreen", "darkslateblue", "darkslategray", "darkslategrey", "darkturquoise",
    "darkviolet", "deeppink", "deepskyblue", "dimgray", "dimgrey", "dodgerblue",
    "firebrick", "floralwhite", "forestgreen", "fuchsia", "gainsboro", "ghostwhite",
    "gold", "goldenrod", "gray", "green", "greenyellow", "grey", "honeydew", "hotpink",
    "indianred", "indigo", "ivory", "khaki", "lavender", "lavenderblush", "lawngreen",
    "lemonchiffon", "lightblue", "lightcoral", "lightcyan", "lightgoldenrodyellow",
    "lightgray", "lightgreen", "lightgrey", "lightpink", "lightsalmon", "lightseagreen",
    "lightskyblue", "lightslategray", "lightslategrey", "lightsteelblue", "lightyellow",
    "lime", "limegreen", "linen", "magenta", "maroon", "mediumaquamarine", "mediumblue",
    "mediumorchid", "mediumpurple", "mediumseagreen", "mediumslateblue", "mediumspringgreen",
    "mediumturquoise", "mediumvioletred", "midnightblue", "mintcream", "mistyrose",
    "moccasin", "navajowhite", "navy", "oldlace", "olive", "olivedrab", "orange",
    "orangered", "orchid", "palegoldenrod", "palegreen", "paleturquoise", "palevioletred",
    "papayawhip", "peachpuff", "peru", "pink", "plum", "powderblue", "purple",
    "rebeccapurple", "red", "rosybrown", "royalblue", "saddlebrown", "salmon",
    "sandybrown", "seagreen", "seashell", "sienna", "silver", "skyblue", "slateblue",
    "slategray", "slategrey", "snow", "springgreen", "steelblue", "tan", "teal",
    "thistle", "tomato", "transparent", "turquoise", "violet", "wheat", "white",
    "whitesmoke", "yellow", "yellowgreen"
]);

const HEX_REGEX = /^#(?:[0-9a-fA-F]{3,4}){1,2}$/;
const RGB_REGEX = /^rgba?\(\s*(?:\d+(?:\.\d+)?%?|\.\d+%?)\s*[, ]\s*(?:\d+(?:\.\d+)?%?|\.\d+%?)\s*[, ]\s*(?:\d+(?:\.\d+)?%?|\.\d+%?)(?:\s*[,/]\s*(?:\d+(?:\.\d+)?%?|\.\d+%?))?\s*\)$/i;
const HSL_HWB_REGEX = /^(?:hsl|hsla|hwb)\(\s*(?:\d+(?:\.\d+)?(?:deg|rad|turn)?|\.\d+(?:deg|rad|turn)?)\s*[, ]\s*(?:\d+(?:\.\d+)?%|\.\d+%)\s*[, ]\s*(?:\d+(?:\.\d+)?%|\.\d+%)(?:\s*[,/]\s*(?:\d+(?:\.\d+)?%?|\.\d+%?))?\s*\)$/i;

export function isValidColor(str : string) {
    if (typeof str !== 'string') return false;

    const normalized = str.trim().toLowerCase();
    if (normalized === '') return false;

    // Check named keywords first
    if (NAMED_COLORS.has(normalized)) return true;

    // Check hex values
    if (normalized.startsWith('#')) return HEX_REGEX.test(normalized);

    // Check RGB, HSL, and HWB function syntax
    return RGB_REGEX.test(normalized) || HSL_HWB_REGEX.test(normalized);
};