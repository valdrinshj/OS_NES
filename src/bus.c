#include "bus.h"
#include "ppu.h"
#include "cpu.h"


void BusInit(Bus *bus) {
    bus->systemClocks = 0;
    bus->dma_addr = 0x00;
    bus->dma_data = 0x00;
    bus->dma_page = 0x00;
    bus->dma_transfer = false;
    bus->dma_dummy = true;
}

uint8_t CpuRead(Bus *bus, uint16_t addr) {
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

void CpuWrite(Bus *bus, uint16_t addr, uint8_t data) {
    if (CartridgeCpuWrite(bus->cartridge, addr, data)) {

    }
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        MemoryWrite(bus->memory, addr, data);
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF) {
        CpuWriteToPpu(addr, data);
    }
    else if(addr == 0x4014) {
        bus->dma_page = data;
        bus->dma_addr = 0x00;
        bus->dma_transfer = true;
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
    Reset();
    bus->systemClocks = 0;
}

void NesClock(Bus *bus) {
    PpuClock();
    if (bus->systemClocks % 3 == 0) {
        if(bus->dma_transfer) {
            if(bus->dma_dummy) {
                if(bus->systemClocks % 2 == 1) {
                    bus->dma_dummy = false;
                }
            } else {
                if(bus->systemClocks % 2 == 0) {
                    bus->dma_data = Read(bus->dma_page << 8 | bus->dma_addr);
                } else {
                    bus->ppu->pOAM[bus->dma_addr] = bus->dma_data;
                    bus->dma_addr++;

                    if(bus->dma_addr == 0x00) {
                        bus->dma_transfer = false;
                        bus->dma_dummy = true;
                    }
                }

            }
        } else {
            Clock();
        }
    }

    if (bus->ppu->nmi) {
        bus->ppu->nmi = false;  // Reset the nmi flag
        Nmi();
    }
    bus->systemClocks++;
}