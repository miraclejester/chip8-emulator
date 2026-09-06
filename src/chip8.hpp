#pragma once
#include <string>
#include <array>
#include <cstdint>
#include <stack>

#include "constants.hpp"

class Chip8
{
public:
    Chip8();
    
    void loadRom(const std::string& path);
    void cycle(); // fetch, decode, execute
    void tickTimers(); // Call at 60hz
    
    const std::array<uint8_t, DISPLAY_SIZE>& display() const;
    void setKey(uint8_t key, bool pressed);
    bool drawFlag = false;
    
    //Debug methods
    void printMemory(uint16_t start = 0x0, uint16_t end = 0xFFF);
    
private:
    std::array<uint8_t, MEMORY_SIZE> memory{};
    std::array<uint8_t, 16> V{}; // registers V0 .. VF
    uint16_t I = 0;
    uint16_t pc = 0x200;
    std::stack<uint16_t> stack{};
    uint8_t sp = 0;
    uint8_t delayTimer = 0, soundTimer = 0;
    std::array<uint8_t, DISPLAY_SIZE> gfx{};
    std::array <bool, 16> keys{};
};
