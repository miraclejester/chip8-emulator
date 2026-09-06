#include "chip8.hpp"
#include "constants.hpp"
#include <cstdint>
#include <iostream>
#include <format>

Chip8::Chip8()
{
    // Write default fonts to memory
    int index = 0x050;
    for (const uint8_t& byte : DEFAULT_FONT) {
        memory[index] = byte;
        index += 0x1;
    }
}

void Chip8::setKey(uint8_t key, bool pressed)
{
    keys[key] = pressed;
}

void Chip8::printMemory(uint16_t start, uint16_t end)
{
    for (int i = start; i <= end; ++i)
    {
        std::cout << std::format("0x{:02x} ", memory[i]);
    }
    
    std::cout << "\n";
}
