#include "bus.h"
#include "ppu.h"
//initialize Bus and Ppu
static Bus bus;
static Ppu ppu;

void cpuWrite(uint16_t addr, uint8_t data) {
    if(addr >= 0x0000 && addr <= 0x1FFF) {
        bus.cpuRam[addr & 0x7FF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        cpuWriteToPpu(addr & 0x0007, data);
    }
}
uint8_t cpuRead(uint16_t addr, bool bReadOnly) {
    uint8_t data = 0x00;

    if(addr >= 0x0000 && addr <= 0x1FFF) {
        data = bus.cpuRam[addr & 0x7FF];
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        data = cpuReadFromPpu(addr & 0x007, bReadOnly);
    }
    return data;
}