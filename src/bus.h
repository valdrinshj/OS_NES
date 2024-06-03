#ifndef BUS_H
#define BUS_H

#include "memory.h"
#include "cartridge.h"
#include "ppu.h"

typedef struct {
    Ppu2C02 *ppu;
    Memory *memory;
    Cartridge *cartridge;
    uint8_t controllerState[2];
    uint8_t controller[2];
} Bus;

void BusInit(Bus *bus);

uint8_t BusRead(Bus *bus, uint16_t addr);
void BusWrite(Bus *bus, uint16_t addr, uint8_t data);

void NesInsertCartridge(Bus *bus, Cartridge *cartridge);
void NesReset(Bus *bus);
void NesClock(Bus *bus);

#endif  // BUS_H