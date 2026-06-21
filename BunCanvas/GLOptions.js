import lib from "./symbols";

export default class GL {
    static set glfwSwapInterval(v){
        lib.symbols.set_glfswapinterval(v)
    }
}