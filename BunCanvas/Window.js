import lib from "./symbols";

const requestedFrames = []

export default class Window {
	#wndPtr;
	constructor(width, height) {
		lib.symbols.create_window(width,height)
		
		let int = setInterval(()=>{
			if (lib.symbols.should_window_close()) {
				lib.symbols.destroy_window();
				clearInterval(int);
				process.exit(0);
				return
			}
			// console.log("window updating")
			// requestedFrames.forEach(item => {
			// 	item()
			// })
			const pLen = requestedFrames.length
			for (let i = 0; i < pLen; i++) {
				(requestedFrames.shift())()
			}
			lib.symbols.update_window();
			
		},0);
	}
	//Other Implementations to be done...
};

export function requestAnimationFrame(callback) {
	requestedFrames.push(callback);
}