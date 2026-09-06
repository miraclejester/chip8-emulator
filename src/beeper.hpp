#pragma once

#include <cstdint>
#include <SDL_audio.h>

class Beeper
{
public:
    ~Beeper();
    bool init();
    void play();
    void stop();
    
private:
    static void audioCallback(void* userdata, uint8_t* stream, int len);
    void fill(int16_t* buffer, int sampleCount);
    
    SDL_AudioDeviceID device = 0;
    int phase = 0;
    int period = 0;
};
