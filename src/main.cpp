#include "platform.hpp"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[])
{
	std::cout << std::unitbuf;
	Chip8Platform* platform = new Chip8Platform();
	platform->printDiagnostics();
	
	char* base = SDL_GetBasePath();
	const std::string rom = std::string(base ? base : "") + "roms/5-quirks.ch8";
	SDL_free(base);
	
    return platform->runApp(rom);
}