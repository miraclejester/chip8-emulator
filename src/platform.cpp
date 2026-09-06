#include "platform.hpp"

#include <iostream>
#include <SDL2/SDL.h>
#include <cstdio>
#include "chip8.hpp"
#include "constants.hpp"

Chip8Platform::Chip8Platform()
{
    emulator = new Chip8();
}

int Chip8Platform::runApp()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Chip-8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        64 * 20, 32 * 20,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::fprintf(stderr, "CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;	
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::fprintf(stderr, "CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    uint32_t frameStart;
    int frameTime;

    bool running = true;
    while (running) {
        //Frame start calc
        frameStart = SDL_GetTicks();
        
        //Handle events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN)
            {
                SDL_Keycode keyCode = e.key.keysym.sym;
                if (keyCode == SDLK_ESCAPE) {
                    running = false;
                }
                
                if (KEY_MAP.contains(keyCode)) {
                    emulator->setKey(KEY_MAP.at(keyCode), true);
                    std::cout << "Key " << KEY_LABEL_MAP.at(keyCode) << " pressed\n";
                }
            }
            if (e.type == SDL_KEYUP)
            {
                SDL_Keycode keyCode = e.key.keysym.sym;
                if (KEY_MAP.contains(keyCode)) {
                    emulator->setKey(KEY_MAP.at(keyCode), false);
                    std::cout << "Key " << KEY_LABEL_MAP.at(keyCode) << " released\n";
                }
            }
        }
        
        //Update emulator logic
        
        //Render graphics
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        
        //Force to 60hz
        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void Chip8Platform::printDiagnostics() const
{
    std::cout << "Font Memory\n";
    emulator->printMemory(FONT_START_ADDR, (FONT_START_ADDR + FONT_BLOCK_SIZE) - 1);
}
