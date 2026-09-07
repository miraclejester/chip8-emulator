#include <iostream>
#include <string>

#include <SDL.h>

#include "platform.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace
{
    Chip8Platform* g_platform = nullptr;
    constexpr const char* DEFAULT_ROM = "roms/2-ibm-logo.ch8";

    std::string defaultRomPath()
    {
#ifdef __EMSCRIPTEN__
        // Bundled via --preload-file, mounted at / in the virtual filesystem.
        return DEFAULT_ROM;
#else
        char* base = SDL_GetBasePath();
        std::string path = std::string(base ? base : "") + DEFAULT_ROM;
        SDL_free(base);
        return path;
#endif
    }
}

#ifdef __EMSCRIPTEN__

void mainLoop(void* arg)
{
    static int ticks = 0;
    ++ticks;
    static_cast<Chip8Platform*>(arg)->frame();
}

extern "C" {

// Called from TypeScript. Path is inside Emscripten's virtual filesystem:
// either a preloaded ROM ("roms/brix.ch8") or one the page wrote via FS.writeFile.
EMSCRIPTEN_KEEPALIVE int chip8_load_rom(const char* path)
{
    if (!g_platform || !path) return 0;
    return g_platform->loadRom(path) ? 1 : 0;
}

}

#endif

int main(int argc, char* argv[])
{
    std::cout << std::unitbuf;

    g_platform = new Chip8Platform();

    const std::string rom = (argc > 1) ? argv[1] : defaultRomPath();

#ifdef __EMSCRIPTEN__
    if (!g_platform->init())
    {
        return 1;
    }
    g_platform->loadRom(rom);

    // fps 0 -> browser drives the loop off requestAnimationFrame.
    // simulate_infinite_loop 1 -> throws to escape main() so it never returns. Letting
    // main() return lets the runtime exit path bump MainLoop.currentlyRunningMainloop,
    // after which the scheduled frame sees a stale id and silently stops.
    emscripten_set_main_loop_arg(mainLoop, g_platform, 0, 1);

    return 0;
#else
    return g_platform->runApp(rom);
#endif
}
