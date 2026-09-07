# CHIP-8 web front end

TypeScript + Vite front end for the CHIP-8 emulator, which is compiled from the
C++ in `../src` to WebAssembly with Emscripten.

## Licensing

This project is MIT licensed — see `../LICENSE` at the repository root, which
covers both this front end and the emulator source in `../src`.

That covers the code only. The bundled ROMs are
third-party and carry their own terms:

- The CHIP-8 logo, IBM logo and test ROMs come from
  [Timendus/chip8-test-suite](https://github.com/Timendus/chip8-test-suite) and are
  GPL-3.0. Their licence text is at `../roms/LICENSE-chip8-test-suite`.
- Brix, Breakout and the particle demo were obtained from
  [loktar00/chip8](https://github.com/loktar00/chip8), which states no licence.

The ROMs are data loaded at runtime, not code linked into the emulator, so the
GPL-3.0 terms on the test suite are not treated as extending to the source here.

## Build

Compile the WebAssembly first (from the repository root, with emsdk on PATH):

    emcmake cmake -B build-web
    cmake --build build-web

That writes `chip8.js`, `chip8.wasm` and `chip8.data` into `public/wasm/`. Then:

    npm install
    npm run dev
