#include "platform.hpp"

#include <cstdio>
#include <iostream>

#include <SDL.h>

#include "chip8.hpp"
#include "constants.hpp"

namespace
{
    constexpr int FPS = 60;
    constexpr int INSTRUCTIONS_PER_SECOND = 700;
    constexpr int INSTRUCTIONS_PER_FRAME = INSTRUCTIONS_PER_SECOND / FPS;
    constexpr int FRAME_DELAY = 1000 / FPS;
}

Chip8Platform::Chip8Platform()
{
    emulator = new Chip8();
}

Chip8Platform::~Chip8Platform()
{
    shutdown();
    delete emulator;
}

bool Chip8Platform::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Chip-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH * DISPLAY_SCALE, DISPLAY_HEIGHT * DISPLAY_SCALE,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::fprintf(stderr, "CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::fprintf(stderr, "CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_RenderSetLogicalSize(renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT);

    if (!beeper.init()) {
        std::fprintf(stderr, "Beeper init failed: %s\n", SDL_GetError());
        return false;
    }

    running = true;
    return true;
}

bool Chip8Platform::loadRom(const std::string& romPath)
{
    return emulator->loadRom(romPath);
}

void Chip8Platform::frame()
{
    const uint32_t frameStart = SDL_GetTicks();

    //Handle events
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
        if (e.type == SDL_KEYDOWN)
        {
            SDL_Keycode keyCode = e.key.keysym.sym;
            if (KEY_MAP.contains(keyCode)) {
                emulator->setKey(KEY_MAP.at(keyCode), true);
            }
        }
        if (e.type == SDL_KEYUP)
        {
            SDL_Keycode keyCode = e.key.keysym.sym;
            if (KEY_MAP.contains(keyCode)) {
                emulator->setKey(KEY_MAP.at(keyCode), false);
            }
        }
    }

    //Update emulator logic
    emulator->drawFlag = false;
    emulator->tickTimers();

    if (emulator->isBeeping())
    {
        beeper.play();
    } else
    {
        beeper.stop();
    }

    for (int i = 0; i < INSTRUCTIONS_PER_FRAME; ++i)
    {
        emulator->cycle();
    }

    //Render graphics
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const auto& gfx = emulator->display();
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (size_t y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (size_t x = 0; x < DISPLAY_WIDTH; ++x) {
            if (gfx[y * DISPLAY_WIDTH + x]) {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }

    SDL_RenderPresent(renderer);

#ifndef __EMSCRIPTEN__
    //Force to 60hz. On the web requestAnimationFrame already paces us.
    const int frameTime = static_cast<int>(SDL_GetTicks() - frameStart);
    if (FRAME_DELAY > frameTime)
    {
        SDL_Delay(FRAME_DELAY - frameTime);
    }
#else
    (void)frameStart;
#endif
}

int Chip8Platform::runApp(std::string romPath)
{
    if (!init())
    {
        return 1;
    }

    if (!loadRom(romPath))
    {
        return 1;
    }

    while (running)
    {
        frame();
    }

    shutdown();
    return 0;
}

void Chip8Platform::shutdown()
{
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window) { SDL_DestroyWindow(window); window = nullptr; }
    SDL_Quit();
}

void Chip8Platform::printDiagnostics() const
{
    std::cout << "Font Memory\n";
    emulator->printMemory(FONT_START_ADDR, (FONT_START_ADDR + FONT_BLOCK_SIZE) - 1);
}
