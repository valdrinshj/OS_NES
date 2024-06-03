#include "bus.h"
#include "ppu.h"
#include "cpu.h"

static uint32_t systemClocks = 0;

void BusInit(Bus *bus) {
    // TODO: See how this can fit into the code
}

uint8_t BusRead(Bus *bus, uint16_t addr) {
    uint8_t data = 0x00;
    if (CartridgeCpuRead(bus->cartridge, addr, &data)) {

    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        return MemoryRead(bus->memory, addr);
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        return CpuReadFromPpu(addr, false);
    }
    else if (addr >= 0x4016 && addr <= 0x4017) {
        data = (bus->controllerState[addr & 0x0001] & 0x80) > 0;
        bus->controllerState[addr & 0x0001] <<= 1;
    }
    return data;
}

void BusWrite(Bus *bus, uint16_t addr, uint8_t data) {
    if (CartridgeCpuWrite(bus->cartridge, addr, data)) {

    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        MemoryWrite(bus->memory, addr, data);
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        CpuWriteToPpu(addr, data);
    }
    else if (addr >= 0x4016 && addr <= 0x4017) {
        bus->controllerState[addr & 0x0001] = bus->controller[addr & 0x0001];
    }
}

void NesInsertCartridge(Bus *bus, Cartridge *cartridge) {
    bus->cartridge = cartridge;
    PpuConnectCartridge(cartridge);
}

void NesReset(Bus *bus) {
    CpuReset();
    systemClocks = 0;
}

void NesClock(Bus *bus) {
    PpuClock();
    if (systemClocks % 3 == 0) {
        CpuClock();
    }

    if (bus->ppu->nmi) {
        bus->ppu->nmi = false;  // Reset the nmi flag
        CpuNmi();
    }
    systemClocks++;
}