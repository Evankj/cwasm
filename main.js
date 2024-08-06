const canvas = document.createElement("canvas");
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

const MAX_MEMORY = 10;

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

function walloc(size) {
  const memView = new DataView()
}

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
    debug: (val) => console.log(val),
    setPlayerPos: (x, y) => {
      ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
      ctx.fillStyle = "red";
      ctx.fillRect(x, y, 10, 10);
    },
    wasm_malloc: function(size) {
      let currentPointer = bumpPointer + heapBase;
      bumpPointer += size;
      if (bumpPointer > memory.buffer.byteLength) {
        return 0; // Indicate memory allocation failure
      }
      return currentPointer;
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

    let previousTime = 0.0;

    const loop = time => {
      // Compute the delta-time against the previous time
      const dt = time - previousTime;

      // Update the previous time
      previousTime = time;

      tick(dt);

      // Repeat
      window.requestAnimationFrame(loop);
    };

    // Launch
    window.requestAnimationFrame(time => {
      previousTime = time;

      window.requestAnimationFrame(loop);
    });


    // const memView = new DataView(instance.exports.memory.buffer, 1);
    // console.log(memView.getInt32(0, true));
  })
  .catch(err => console.error('Failed to load wasm file:', err));
