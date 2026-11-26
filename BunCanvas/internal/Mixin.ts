export type Constructor<T = {}> = new (...args: any[]) => T;

export function Mixin<TBase extends Constructor, TMixins extends Array<(base: any) => any>>(
    Base: TBase,
    ...mixins: TMixins
) {
    return mixins.reduce((acc, mixin) => mixin(acc), Base) as TBase &
        UnionToIntersection<ReturnType<TMixins[number]>>;
}

type UnionToIntersection<U> = (U extends any ? (k: U) => void : never) extends (
    k: infer I
) => void
    ? I
    : never;