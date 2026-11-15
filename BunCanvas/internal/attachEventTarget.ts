export function attachEventTarget<T extends Event | Event>(event: T, target: unknown): T {
	Object.defineProperty(event, "target", {
		value: target,
		enumerable: true,
		configurable: true,
	});
	return event;
}