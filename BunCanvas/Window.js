import { Canvas } from "./Canvas";
import lib from "./symbols";
const requestedFrames = []

export default class Window {
	#wndPtr;

	#lastMouseX = 0
	#lastMouseY = 0

	constructor(width, height) {
		lib.symbols.create_window(width,height)
		
		let int = setInterval(()=>{
			if (lib.symbols.should_window_close()) {
				lib.symbols.destroy_window();
				clearInterval(int);
				process.exit(0);
				return
			}
			
			lib.symbols.update_window();
			const pLen = requestedFrames.length
			for (let i = 0; i < pLen; i++) {
				(requestedFrames.shift())()
			}
		},0);
	}

	append(canvas){
		if (canvas instanceof Canvas) {
			lib.symbols.canvas_append(canvas.__iptr)
		}
	}
	//Other Implementations to be done...
};

export function requestAnimationFrame(callback) {
	requestedFrames.push(callback);
}