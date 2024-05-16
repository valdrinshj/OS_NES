#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include <stdbool.h>
#include "ppu.h"
#include "cartridge.h"


typedef struct{
    // Cpu cpu;
    Ppu *ppu;
    uint8_t cpuRam[2048];
    Cartridge *cartridge;

} Bus;

// A count of how many clocks have passed
// uint32_t nSystemClockCounter = 0;

//Functions

void busWrite(Bus *bus, uint16_t addr, uint8_t data);
uint8_t busRead(Bus *bus, uint16_t addr);

void NesInsertCartridge(Bus *bus, Cartridge *cartridge);
void NesReset(Bus *bus);
void NesClock(Bus *bus);
#endif
