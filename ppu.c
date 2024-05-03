#include "bus.h"
#include "ppu.h"

static Ppu ppu = {0};

uint8_t cpuReadFromPpu(uint16_t addr, bool bReadOnly) {
    uint8_t data = 0x00;

    switch (addr) {
        case 0x0000: // Control
            break;
        case 0x0001: // Mask
            break;
        case 0x0002: // Status
            break;
        case 0x0003: // OAM Address
            break;
        case 0x0004: // OAM Data
            break;
        case 0x0005: // Scroll
            break;
        case 0x0006: // PPU Address
            break;
        case 0x0007: // PPU Data
            break;
    }

    return data;
}
void cpuWriteToPpu(uint16_t addr, uint8_t data) {
    switch (addr)
    {
        case 0x0000: // Control
            break;
        case 0x0001: // Mask
            break;
        case 0x0002: // Status
            break;
        case 0x0003: // OAM Address
            break;
        case 0x0004: // OAM Data
            break;
        case 0x0005: // Scroll
            break;
        case 0x0006: // PPU Address
            break;
        case 0x0007: // PPU Data
            break;
    }
}

uint8_t ppuRead(uint16_t addr, bool rReadOnly) {
    uint8_t data = 0x00;
    addr &= 0x3FFF;  //placeholders

    return data;
}
void ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;
}
void PpuConnectCartridge(Cartridge *cartridge) {
    ppu.cartridge = cartridge;
}
