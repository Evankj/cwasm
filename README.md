This repo just holds some experiments I've been doing with C and WebAssembly.



# notes for myself:

## nice to haves:
### Namespaces
These can be achieved through function pointers within a struct. See [math.h & math.c](./c/math) for an example.

### Interfaces
Strongly typed interfaces can sort of be achieved through ["type classes"](https://github.com/TotallyNotChase/c-iterators/blob/master/Typeclass%20Pattern.md) and are relatively easy to implement with some helper macros, but for simple use cases it is probably easier to just have a single implementation and go through the effort of generating a type class only when the need arises. My main use case for this currently was for implementing a generalised Allocator type class that has access to itself (to adjust the offset in an arena allocator for example), but I haven't needed anything besides arena allocators so far when doing game dev with the WASM linear memory.

### Testing
This isn't so nice, but we can write some tests in C, compile them to WASM and execute our WASM module with Node. The important thing is that, since we're compiling with `-nostdlib`, we don't have `exit` or similar. 

