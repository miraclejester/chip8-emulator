class Chip8
{
public:
    void loadRom(const std::string& path);
    void cycle(); // fetch, decode, execute
    void tickTimers(); // Call at 60hz
    
    const std::array<uint8_t, 64*32>& display() const;
    void setKey(uint8_t key, bool pressed);
    bool drawFlag = false;
    
private:
    std::array<uint8_t, 4096> memory{};
    std::array<uint8_t, 16> V{}; // registers V0 .. VF
    uint16_t I = 0;
    uint16_t pc = 0x200;
    std::array<uint16_t, 16> stack{};
    uint8_t sp = 0;
    uint8_t delayTimer = 0, soundTimer = 0;
    std::array<uint8_t, 64*32> gfx{};
    std::array <bool, 16> keys{};
};