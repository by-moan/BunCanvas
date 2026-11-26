// export function setHandler<T extends Event, ETarget extends EventTarget>(
//     target : ETarget,
//     type: string,
//     current: ((evt: T) => any) | null,
//     next: ((evt: T) => any) | null
// ) {
//     if (current) {
//         target.removeEventListener(type, current as EventListener);
//     }
//     if (next) {
//         target.addEventListener(type, next as EventListener);
//     }
//     return next;
// }

export function setHandler(
    target,
    type,
    current,
    next
) {
    if (current) {
        target.removeEventListener(type, current);
    }
    if (next) {
        target.addEventListener(type, next);
    }
    return next;
}