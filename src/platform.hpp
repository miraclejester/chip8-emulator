#pragma once

class Chip8;

class Chip8Platform
{
public:
    Chip8Platform();
    
    void printDiagnostics() const;
    
private:
    Chip8* emulator;
};
