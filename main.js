const canvas = document.createElement("canvas");
canvas.width = 1920;
canvas.height = 1080;
document.body.appendChild(canvas);
const ctx = canvas.getContext("2d");

/**
  * @type {WebAssembly.Instance | undefined}
  */
let wasm = undefined;
let heapBase = -1;

function myJavaScriptFunction(x, y, w, h) {
  ctx.fillStyle = "red";
  ctx.fillRect(x, y, w, h);
}

const MAX_MEMORY = 65536;

const memory = new WebAssembly.Memory({
  initial: MAX_MEMORY,
  maximum: MAX_MEMORY,
  shared: true
});

const bumpAllocator = {
  start: 0,
  offset: 0,
  maxSize: MAX_MEMORY * 64
}

// function walloc(size) {
//   const memView = new DataView()
// }

let bumpPointer = 0;

function cstrlen(mem, ptr) {
  let len = 0;
  while (mem[ptr] != 0) {
    len++;
    ptr++;
  }
  return len;
}

function cstr_by_ptr(mem_buffer, ptr) {
  const mem = new Uint8Array(mem_buffer);
  const len = cstrlen(mem, ptr);
  const bytes = new Uint8Array(mem_buffer, ptr, len);
  return new TextDecoder().decode(bytes);
}

const importObject = {
  env: {
    // memory: memory,
    myJavaScriptFunction: (x, y, w, h) => myJavaScriptFunction(x, y, w, h),
    debugi32: (val) => console.log(val),
    debugf32: (val) => console.log(val),
    setPlayerPos: (x, y) => {
      ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
      ctx.fillStyle = "red";
      ctx.fillRect(x, y, 10, 10);
    },
    drawf32: (val) => {
      console.log(val);
      // ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
      // ctx.fillStyle = "blue";
      // ctx.fillText(val, 0, 50);
    },

    breakpoint: () => {
      debugger;
    },

    wasm_malloc: function(size) {
      let currentPointer = bumpPointer + heapBase;
      bumpPointer += size;
      console.log(`Base ptr: ${currentPointer}, Cur ptr: ${currentPointer + bumpPointer}, Remaining space: ${memory.buffer.byteLength - (currentPointer + bumpPointer)}`);
      if (bumpPointer > memory.buffer.byteLength) {
        console.log("MEMORY OVERRUN");
        return 0; // Indicate memory allocation failure
      }
      return currentPointer;
    },

    wasm_memset: (ptr, size, val) => {
      const memView = new DataView(wasm.exports.memory.buffer, ptr, size);
      for (let i = 0; i < size; i++) {
        memView.setUint8(i, val);
      }
    },

    print: (start, size) => {

      const memView = new DataView(wasm.exports.memory.buffer, start, size);
      let str = "";
      for (let i = 0; i < size; i++) {
        const char = String.fromCharCode(memView.getUint8(i));
        str += char;
      }

      console.log(str);
    },
    printCStr: (ptr) => {
      const buffer = wasm.exports.memory.buffer;
      let str = cstr_by_ptr(buffer, ptr);
      console.log(str);
    }


  },

  js: { mem: memory },
};


fetch('test.wasm')
  .then(response => response.arrayBuffer())
  .then(bytes => WebAssembly.instantiate(bytes, importObject))
  .then(results => {
    const instance = results.instance;
    wasm = instance;
    heapBase = instance.exports.__heap_base;
    const test = instance.exports.test;
    test();
    const setInputState = instance.exports.setInputState;
    const tick = instance.exports.tick;

    addEventListener('keypress', (e) => {
      switch (e.key) {
        case 'w':
          setInputState(1, 1);
          break;
        case 'a':
          setInputState(2, 1);
          break;
        case 's':
          setInputState(3, 1);
          break;
        case 'd':
          setInputState(4, 1);
          break;
      }
    });
    addEventListener('keyup', (e) => {
      switch (e.key) {
        case 'w':
          setInputState(1, 0);
          break;
        case 'a':
          setInputState(2, 0);
          break;
        case 's':
          setInputState(3, 0);
          break;
        case 'd':
          setInputState(4, 0);
          break;
      }
    });


    const fps = 60;
    const fpsInterval = 1000 / fps;
    let then = performance.now();



    const loop = () => {
      requestAnimationFrame(loop);

      const now = performance.now();
      const elapsed = (now - then);

      if (elapsed >= fpsInterval) {
        then = now - (elapsed % fpsInterval);

        let dt = elapsed / 100;
        // let fps = 1000 / elapsed;

        tick(dt);
      }
    };

    loop();


    // const memView = new DataView(instance.exports.memory.buffer, 1);
    // console.log(memView.getInt32(0, true));
  })
  .catch(err => console.error('Failed to load wasm file:', err));
