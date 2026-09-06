#pragma once

#include <array>
#include <map>
#include <SDL_keycode.h>

const std::map<SDL_Keycode, uint8_t> KEY_MAP = {
    { SDLK_1, 0 },
    { SDLK_2, 1 },
    { SDLK_3, 2 },
    { SDLK_4, 3 },
    { SDLK_q, 4 },
    { SDLK_w, 5 },
    { SDLK_e, 6 },
    { SDLK_r, 7 },
    { SDLK_a, 8 },
    { SDLK_s, 9 },
    { SDLK_d, 10 },
    { SDLK_f, 11 },
    { SDLK_z, 12 },
    { SDLK_x, 13 },
    { SDLK_c, 14 },
    { SDLK_v, 15 }
};

const std::map<SDL_Keycode, std::string> KEY_LABEL_MAP = {
    { SDLK_1, "1" },
    { SDLK_2, "2" },
    { SDLK_3, "3" },
    { SDLK_4, "C" },
    { SDLK_q, "4" },
    { SDLK_w, "5" },
    { SDLK_e, "6" },
    { SDLK_r, "D" },
    { SDLK_a, "7" },
    { SDLK_s, "8" },
    { SDLK_d, "9" },
    { SDLK_f, "E" },
    { SDLK_z, "A" },
    { SDLK_x, "0" },
    { SDLK_c, "B" },
    { SDLK_v, "F" }
};

inline constexpr int DISPLAY_SCALE = 20;
inline constexpr size_t DISPLAY_WIDTH = 64;
inline constexpr size_t DISPLAY_HEIGHT = 32;
inline constexpr size_t DISPLAY_SIZE = DISPLAY_WIDTH*DISPLAY_HEIGHT; // 64*32
inline constexpr size_t MEMORY_SIZE = 4096;

inline constexpr uint16_t FONT_START_ADDR = 0x050;
inline constexpr uint16_t FONT_BLOCK_SIZE = 0x050;

inline constexpr std::array<uint8_t, FONT_BLOCK_SIZE> DEFAULT_FONT = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
