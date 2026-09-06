#include "beeper.hpp"

#include "constants.hpp"

bool Beeper::init()
{
    SDL_AudioSpec spec{};
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 512;
    spec.callback = &audioCallback;
    spec.userdata = this;
    
    SDL_AudioSpec obtainedSpec;
    
    device = SDL_OpenAudioDevice(NULL, 0, &spec, &obtainedSpec, 0);
    
    if (device == 0)
    {
        return false;
    }
    
    period = obtainedSpec.freq / BEEP_FREQUENCY;
    return true;
}

void Beeper::play()
{
    SDL_PauseAudioDevice(device, 0);
}

void Beeper::stop()
{
    SDL_PauseAudioDevice(device, 1);
}

void Beeper::audioCallback(void* userdata, uint8_t* stream, int len)
{
    auto* self = static_cast<Beeper*>(userdata);
    self->fill(reinterpret_cast<int16_t*>(stream), len / static_cast<int>(sizeof(int16_t)));
}

void Beeper::fill(int16_t* buffer, int sampleCount)
{
    for (int i = 0; i < sampleCount; ++i)
    {
        buffer[i] = (phase < period / 2) ? BEEP_AMPLITUDE : -BEEP_AMPLITUDE;
        phase = (phase + 1) % period;
    }
}

Beeper::~Beeper()
{
    if (device != 0)
    {
        SDL_CloseAudioDevice(device);
    }
}
