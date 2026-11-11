export const INTERNAL_TOKEN = Object.freeze({});

export const throwIllegalConstructor = (object: any) => {
    throw new TypeError(`Failed to construct '${object.constructor.name}': Illegal constructor`)
}