#! /bin/bash
clang -g --target=wasm32 -I./c -nostdlib -Wl,--no-entry -Wl,--export-all -v -o test.wasm $(find ./c -name "*.c")
