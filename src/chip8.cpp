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
    int index = FONT_START_ADDR;
    for (const uint8_t& byte : DEFAULT_FONT) {
        memory[index] = byte;
        index += 0x1;
    }
    
    std::random_device rd;
    randomGen = std::mt19937(rd());
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

void Chip8::tickTimers()
{
    delayTimer -= (delayTimer > 0) ? 1 : 0;
    soundTimer -= (soundTimer > 0) ? 1 : 0;
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
    uint16_t result = static_cast<uint16_t>(((memory[pc]) << 8) | memory[pc+1]);
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
            if (y == 0xE)
            {
                // 00EE: Return from procedure (go to the address at the top of the stack)
                if (n == 0xE)
                {
                    pc = stack.top();
                    stack.pop();
                } else if (n == 0x0)
                {
                    // 00E0: Clear screen
                    gfx.fill(false);   
                }
            }
            break;
        case 0x1:
            // 1NNN: Jump
            pc = nnn;
            break;
        case 0x2:
            // 2NNN: Call subroutine at NNN
            stack.push(pc);
            pc = nnn;
            break;
        case 0x3:
            // 3XNN: Skip if VX == NN
            pc += (V[x] == nn) ? 2 : 0;
            break;
        case 0x4:
            // 4XNN: Skip if VX != NN
            pc += (V[x] != nn) ? 2 : 0;
            break;
        case 0x5:
            // 5XY0: Skip if VX == VY
            pc += (V[x] == V[y]) ? 2 : 0;
            break;
        case 0x6:
            // 6XNN: Set Register X to NN
            V[x] = nn;
            break;
        case 0x7:
            // 7XNN: Add NN to Register X
            V[x] += nn;
            break;
        case 0x8:
        {
            switch (n)
            {
            // 8XY0: Set VX to VY
            case 0x0:
                V[x] = V[y];
                break;
            // 8XY1: Set VX to VX | VY
            case 0x1:
                V[x] = V[x] | V[y];
                break;
            // 8XY2: Set VX to VX & VY
            case 0x2:
                V[x] = V[x] & V[y];
                break;
            // 8XY3: Set VX to VX ^ VY (XOR)
            case 0x3:
                V[x] = V[x] ^ V[y];
                break;
            // 8XY4: Set VX to VX + VY
            case 0x4:
                {
                    uint8_t prevX = V[x];
                    V[x] = static_cast<uint8_t>(V[x] + V[y]);
                    V[0xF] = (V[x] < prevX) ? 1 : 0;
                    break;
                }
            // 8XY5: Set VX to VX - VY
            case 0x5:
                V[0xF] = (V[x] >= V[y]) ? 1 : 0; 
                V[x] = V[x] - V[y];
                break;
            // 8XY6: Shift right (Modern)
            case 0x6:
                V[0xF] = V[x] & 1;
                V[x] = V[x] >> 1;
                break;
            // 8XY7: Set VX to VY - VX
            case 0x7:
                V[0xF] = (V[y] >= V[x]) ? 1 : 0; 
                V[x] = V[y] - V[x];
                break;
            // 8XYE: Shift left (Modern)
            case 0xE:
                V[0xF] = (V[x] >> 7) & 1;
                V[x] = static_cast<uint8_t>(V[x] << 1);
                break;
            default:
                break;
            }
            break;
        }        
        case 0x9:
            // 9XY0: Skip if VX != VY
            pc += (V[x] != V[y]) ? 2 : 0;
            break;
        case 0xA:
            // ANNN: Set index register to NNN
            I = nnn;
            break;
        case 0xB:
            // BNNN: Jump with offset (Classic)
            pc = nnn + V[0];
            break;
        case 0xC:
            // CXNN: Random number
            V[x] = getRandom() & nn;
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
        case 0xE:
            if (nn == 0x9E)
            {
                // EX9E: Skip if key
                pc += (keys[KEY_HEX_MAP.at(x)]) ? 2 : 0;
            } else if (nn == 0xA1)
            {
                // EXA1: Skip if not key
                pc += (!keys[KEY_HEX_MAP.at(x)]) ? 2 : 0;
            }
            break;
        case 0xF:
            switch (nn)
            {
                // FX07: Store value of delayTimer in VX
                case 0x07:
                    V[x] = delayTimer;
                    break;
                // FX15: Set the delayTimer to VX
                case 0x15:
                    delayTimer = V[x];
                    break;
                // FX18: Set the soundTimer to VX
                case 0x18:
                    soundTimer = V[x];
                    break;
                // FX1E: Add VX to index
                case 0x1E:
                    I += V[x];
                    if (I > 0xFFF)
                    {
                        V[0xF] = 1;
                    }
                    break;
                // FX0A: Get key. Stops execution until a key is pressed
                case 0x0A:
                {
                    int pressedIndex = -1;
                    for (int i = 0; i<keys.size(); ++i)
                    {
                        if (keys[i])
                        {
                            pressedIndex = i;
                            break;
                        }
                    }
            
                    if (pressedIndex <= -1)
                    {
                        pc -= 2;
                    } else
                    {
                        V[x] = KEY_HEX_MAP_REVERSE.at(static_cast<uint8_t>(pressedIndex));
                    }
                    break;    
                }
                // FX29: Font character. Point index to the address of a character (stored in VX)
                case 0x29:
                    I = FONT_START_ADDR + V[x];
                    break;
                // FX33: Binary-coded decimal conversioo
                case 0x33:
                {
                    uint8_t num = V[x];
                    memory[I] = num / 100;
                    memory[I+1] = (num % 100) / 10;
                    memory[I+2] = num % 10;
                    break;
                }
                // FX55: Store registers to memory (modern)
                case 0x55:
                {
                    uint16_t index = I;
                    for (uint8_t i = 0; i <= x; ++i)
                    {
                        memory[index] = V[i];
                        index += 1;
                    }
                    break;
                }
                // FX65: Load registers from memory (modern)
                case 0x65:
                {
                    uint16_t index = I;
                    for (uint8_t i = 0; i <= x; ++i)
                    {
                        V[i] = memory[index];
                        index += 1;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
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

uint8_t Chip8::getRandom()
{
    std::uniform_int_distribution<int> distrib(0, 255);
    return static_cast<uint8_t>(distrib(randomGen));
}
