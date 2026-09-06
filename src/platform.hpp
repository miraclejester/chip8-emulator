#pragma once

#include <string>

class Chip8;

class Chip8Platform
{
public:
    Chip8Platform();
    
    int runApp(std::string romPath);
    void printDiagnostics() const;
    
private:
    Chip8* emulator;
};
