#include "chip8.hpp"
#include "constants.hpp"
#include <cstdint>
#include <iostream>
#include <format>
#include <fstream>
#include <vector>

Chip8::Chip8()
{
    // Write default fonts to memory
    int index = 0x050;
    for (const uint8_t& byte : DEFAULT_FONT) {
        memory[index] = byte;
        index += 0x1;
    }
}

bool Chip8::loadRom(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    pc = 0x200;
    
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << path << "\n";
        return false;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        uint16_t i = pc;
        for (uint8_t byte : buffer)
        {
            memory[i] = byte;
            i += 1;
        }
    }
    return true;
}

void Chip8::cycle()
{
    execute(fetch());
}

const std::array<bool, DISPLAY_SIZE>& Chip8::display() const
{
    return gfx;
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

uint16_t Chip8::fetch()
{
    uint16_t result = static_cast<uint16_t>((memory[pc]) << 8) | memory[pc+1];
    pc += 0x002;
    return result;
}

void Chip8::execute(uint16_t instruction)
{
    uint8_t opcode = instruction >> 12;
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    uint8_t n = instruction & 0xF;
    uint8_t nn = instruction & 0xFF;
    uint16_t nnn = instruction & 0xFFF;
    
    switch (opcode)
    {
        case 0x0:
            // 00E0: Clear screen
            if (y == 0xE)
            {
                gfx.fill(false);
            }
            break;
        case 0x1:
            // 1NNN: Jump
            pc = nnn;
            break;
        case 0x6:
            // 6XNN: Set Register X to NN
            V[x] = nn;
            break;
        case 0x7:
            // 7XNN: Add NN to Register X
            V[x] += nn;
            break;
        case 0xA:
            // ANNN: Set index register to NNN
            I = nnn;
            break;
        case 0xD:
        {
            // DXYN: Display
            uint8_t yCoord = V[y] & (DISPLAY_HEIGHT - 1); // VY mod 32
            V[0xF] = 0;
            for (uint8_t i = 0; i < n; ++i)
            {
                uint8_t spriteByte = memory[I + i];
                uint8_t xCoord = V[x] & (DISPLAY_WIDTH -1); // VX mod 64
                for (int j = 7; j >= 0; --j)
                {
                    bool bit = (spriteByte >> j) & 1;
                    bool isOn = isPixelOn(xCoord, yCoord);
                    if (bit && isOn)
                    {
                        setPixel(xCoord, yCoord, false);
                        V[0xF] = 1;
                    } else if (bit && !isOn)
                    {
                        setPixel(xCoord, yCoord, true);
                    }
                    if (xCoord == DISPLAY_WIDTH - 1)
                    {
                        break;
                    }
                    xCoord += 1;
                }
                if (yCoord == DISPLAY_HEIGHT - 1)
                {
                    break;
                }
                yCoord +=1;
            }
            drawFlag = true;    
            break;
        }
        default:
            std::cout << "Unknown opcode\n";
            break;
    }
}

bool Chip8::isPixelOn(uint8_t x, uint8_t y)
{
    return gfx[(y * DISPLAY_WIDTH) + x];
}

void Chip8::setPixel(uint8_t x, uint8_t y, bool v)
{
    gfx[(y* DISPLAY_WIDTH) + x] = v;
}
