#include "bus.h"

#include "cpu.h"
#include "ppu.h"

static uint32_t nSystemClockCounter = 0;

void busWrite(Bus *bus, uint16_t addr, uint8_t data) {
    if(addr >= 0x0000 && addr <= 0x1FFF) {
        bus->cpuRam[addr & 0x7FF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        cpuWriteToPpu(addr & 0x0007, data);
    }
}
uint8_t busRead(Bus *bus, uint16_t addr) {
    uint8_t data = 0x00;

    if(addr >= 0x0000 && addr <= 0x1FFF) {
        data = bus->cpuRam[addr & 0x7FF];
    } else if (addr >= 0x2000 && addr <= 0x3FFF) {
        data = cpuReadFromPpu(addr & 0x007, false);
    }
    return data;
}
void NesInsertCartridge(Bus *bus, Cartridge *cartridge) {
    bus->cartridge = cartridge;
    PpuConnectCartridge(cartridge);
}
void NesReset(Bus *bus) {
    cpu_reset();
    nSystemClockCounter = 0;
}
void NesClock(Bus *bus) {
    PpuClock();
    if(nSystemClockCounter % 3 == 0) {
        cpu_clock();
    }
    if(bus->ppu->nmi) {
        bus->ppu->nmi = false;
        cpu_non_mask_interrupt();
    }
    nSystemClockCounter++;
}