#include "platform.hpp"

#include <iostream>

#include "chip8.hpp"
#include "constants.hpp"

Chip8Platform::Chip8Platform()
{
    emulator = new Chip8();
}

void Chip8Platform::printDiagnostics() const
{
    std::cout << "Font Memory\n";
    emulator->printMemory(FONT_START_ADDR, (FONT_START_ADDR + FONT_BLOCK_SIZE) - 1);
}
