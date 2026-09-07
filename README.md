# CHIP-8 Emulator

A CHIP-8 interpreter written in C++20 with SDL2, running both as a native desktop
application and in the browser via WebAssembly.

**[Play it in your browser (controls only work on Desktop) →](https://miraclejester.github.io/chip8-emulator/)**

## Features

- All 35 instructions of the original CHIP-8 instruction set
- 64×32 monochrome display with XOR sprite drawing and collision detection
- 16-key hex keypad, delay and sound timers at 60Hz
- Square-wave beeper driven by the sound timer
- Runs at 60fps with roughly 700 instructions per second
- The same source builds natively and to WebAssembly

### Implementation notes

This interpreter follows the modern (SUPER-CHIP) behaviour:

- `8XY6` / `8XYE` shift `VX` in place rather than shifting `VY` into `VX`
- `FX55` / `FX65` leave the index register `I` unmodified
- `BNNN` jumps to `NNN + VX` rather than `NNN + V0`

The bundled quirks ROM (`Test: Quirks`) passes in its SUPER-CHIP modern mode.

## Building

### Native

Requires CMake 3.16+, a C++20 compiler and SDL2. On Windows this project is built with
MSYS2 (UCRT64).

    cmake -B build
    cmake --build build
    ./build/chip8 roms/2-ibm-logo.ch8

The ROM path is optional; without one it loads the IBM logo. ROMs are copied next to the
executable at build time, and the path is resolved relative to the binary, so it works
from any working directory.

### Web

Requires the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
on your `PATH`, plus Node 20+.

    emcmake cmake -B build-web
    cmake --build build-web        # writes web/public/wasm/
    cd web && npm install && npm run dev

`web/` holds the TypeScript front end — ROM picker, file loader and keypad reference. See
[`web/README.md`](web/README.md) for details.

Pushes to `master` deploy automatically via `.github/workflows/deploy.yml`.

## Controls

CHIP-8's hex keypad maps onto the left of a QWERTY keyboard:

    CHIP-8            Keyboard
    1 2 3 C           1 2 3 4
    4 5 6 D    ->     Q W E R
    7 8 9 E           A S D F
    A 0 B F           Z X C V

Brix and Breakout move the paddle with `Q` (left) and `E` (right).

## Layout

    src/          emulator core (chip8.*) and SDL platform layer (platform.*, beeper.*)
    roms/         bundled ROMs, packaged into the web build
    web/          TypeScript + Vite front end
    CMakeLists.txt   native build, plus an if(EMSCRIPTEN) branch for the web target

## Credits

The emulator is my own work. The bundled ROMs are not:

- The CHIP-8 logo, IBM logo and test ROMs come from
  [Timendus/chip8-test-suite](https://github.com/Timendus/chip8-test-suite), licensed
  GPL-3.0. Its licence text ships as [`roms/LICENSE-chip8-test-suite`](roms/LICENSE-chip8-test-suite).
- Brix (commonly attributed to Andreas Gustafsson, 1990), Breakout (Carmelo Cortez, 1979)
  and the particle demo were obtained from
  [loktar00/chip8](https://github.com/loktar00/chip8), which states no licence.

The ROMs are data loaded at runtime rather than code linked into the emulator.

## Licence

MIT — see [LICENSE](LICENSE).
