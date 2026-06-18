import { dlopen, FFIType } from "bun:ffi";

const lib = dlopen("./build/lib.so", {
  PrintTest: {
    args: ["cstring"],
    returns: "void",
  },
});

lib.symbols.PrintTest((new TextEncoder()).encode("hello world!\0"));