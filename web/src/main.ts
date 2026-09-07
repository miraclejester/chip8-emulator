/// <reference types="vite/client" />

interface Chip8Module {
  ccall(
    name: string,
    returnType: string | null,
    argTypes: string[],
    args: unknown[],
  ): number;
  FS: {
    writeFile(path: string, data: Uint8Array): void;
  };
}

type Chip8Factory = (opts: {
  canvas: HTMLCanvasElement;
  locateFile?: (path: string, prefix: string) => string;
  print?: (text: string) => void;
  printErr?: (text: string) => void;
}) => Promise<Chip8Module>;

/** Bundled into the wasm virtual filesystem by --preload-file (see CMakeLists.txt). */
const BUNDLED_ROMS: ReadonlyArray<{ label: string; path: string }> = [
  { label: 'IBM Logo', path: 'roms/2-ibm-logo.ch8' },
  { label: 'CHIP-8 Logo', path: 'roms/1-chip8-logo.ch8' },
  { label: 'Brix', path: 'roms/brix.ch8' },
  { label: 'Breakout', path: 'roms/breakout.ch8' },
  { label: 'Particle Demo', path: 'roms/particle-demo.ch8' },
  { label: 'Test: Corax+', path: 'roms/3-corax+.ch8' },
  { label: 'Test: Flags', path: 'roms/4-flags.ch8' },
  { label: 'Test: Quirks', path: 'roms/5-quirks.ch8' },
  { label: 'Test: Keypad', path: 'roms/6-keypad.ch8' },
  { label: 'Test: Beep', path: 'roms/7-beep.ch8' },
];

const USER_ROM_PATH = '/user-rom.ch8';

const canvas = document.getElementById('canvas') as HTMLCanvasElement;
const startButton = document.getElementById('start') as HTMLButtonElement;
const romSelect = document.getElementById('rom-select') as HTMLSelectElement;
const romFile = document.getElementById('rom-file') as HTMLInputElement;
const statusEl = document.getElementById('status') as HTMLParagraphElement;

let chip8: Chip8Module | null = null;

function setStatus(message: string, isError = false): void {
  statusEl.textContent = message;
  statusEl.classList.toggle('error', isError);
}

function populateRomList(): void {
  for (const rom of BUNDLED_ROMS) {
    const option = document.createElement('option');
    option.value = rom.path;
    option.textContent = rom.label;
    romSelect.append(option);
  }
}

function selectedLabel(): string {
  return romSelect.selectedOptions[0]?.textContent ?? 'ROM';
}

function loadRom(path: string, label: string): void {
  if (!chip8) return;

  const ok = chip8.ccall('chip8_load_rom', 'number', ['string'], [path]);
  if (ok) {
    setStatus(`Running ${label}`);
    canvas.focus();
  } else {
    setStatus(`Could not load ${label}`, true);
  }
}

/**
 * The module is instantiated on a click rather than on page load: browsers refuse
 * to open an audio context without a user gesture, so SDL_OpenAudioDevice would
 * succeed but stay silent. Instantiating runs main() itself, which registers the
 * emulator's frame loop and never returns.
 */
async function boot(): Promise<void> {
  startButton.disabled = true;
  setStatus('Loading…');

  try {
    // Resolve against the page URL so this works both at the site root and
    // under a GitHub Pages subpath.
    const wasmUrl = new URL('wasm/chip8.js', document.baseURI).href;
    const module = await import(/* @vite-ignore */ wasmUrl);
    const createChip8 = module.default as Chip8Factory;

    chip8 = await createChip8({
      canvas,
      // The .wasm resolves next to chip8.js on its own, but the --preload-file data
      // package resolves against the document instead, so it would be requested from
      // the site root. Vite's SPA fallback answers that 404 with index.html, and the
      // packager then slices ROMs out of HTML. Pin every side file to wasm/.
      locateFile: (path: string) => new URL(`wasm/${path}`, document.baseURI).href,
      printErr: (text: string) => console.error(text),
    });


    startButton.remove();
    loadRom(romSelect.value, selectedLabel());
  } catch (error) {
    console.error(error);
    setStatus('Failed to start the emulator. See the browser console.', true);
    startButton.disabled = false;
  }
}

async function onFileChosen(): Promise<void> {
  const file = romFile.files?.[0];
  if (!file || !chip8) return;

  const bytes = new Uint8Array(await file.arrayBuffer());
  if (bytes.length > 4096 - 0x200) {
    setStatus(`${file.name} is too large for CHIP-8 memory.`, true);
    return;
  }

  chip8.FS.writeFile(USER_ROM_PATH, bytes);
  loadRom(USER_ROM_PATH, file.name);
  romSelect.selectedIndex = -1;
}

populateRomList();
startButton.addEventListener('click', boot);
romSelect.addEventListener('change', () => {
  loadRom(romSelect.value, selectedLabel());
});
romFile.addEventListener('change', onFileChosen);
canvas.addEventListener('click', () => canvas.focus());
