#include "platform.hpp"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[])
{
	std::cout << std::unitbuf;
	Chip8Platform* platform = new Chip8Platform();
	platform->printDiagnostics();
	
    return platform->runApp();
}