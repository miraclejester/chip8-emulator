#pragma once

#include <string>

#include "beeper.hpp"

class Chip8;
struct SDL_Window;
struct SDL_Renderer;

class Chip8Platform
{
public:
    Chip8Platform();
    ~Chip8Platform();

    bool init();                              // SDL, window, renderer, beeper
    bool loadRom(const std::string& romPath);
    void frame();                             // one iteration of the emulator loop
    bool isRunning() const { return running; }

    int runApp(std::string romPath);          // native: init + blocking loop
    void printDiagnostics() const;

private:
    void shutdown();

    Chip8* emulator;
    Beeper beeper;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;
};
