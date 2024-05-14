#ifndef INC_6502_MEM_H
#define INC_6502_MEM_H

#include <stdint.h>
#include <stdbool.h>
#include "raylib.h"
#include "cartridge.h"

typedef struct {
    Color *pixel;
    uint16_t width;
    uint16_t height;
} Sprite;

typedef struct {
    Cartridge *cartridge;
    uint8_t nameTable[2][1024];  // Vram that holds nametable information, 2 * 1 Kb nametables
    uint8_t palettes[32]; //Palette information for colors

    uint8_t pattern[2][4096]; // 2 * 4 KB arrays that represent pattern memory, now unnecessary will only be used if we implement our own mapper
}Ppu;

void ppuInit();
//Communication with main bus
uint8_t cpuReadFromPpu(uint16_t addr, bool bReadOnly);
void cpuWriteToPpu(uint16_t addr, uint8_t data);

//Communication with ppu bus
uint8_t ppuRead(uint16_t addr, bool rReadOnly);
void ppuWrite(uint16_t addr, uint8_t data);

// Connect Cartridge to PPU and a clock function
void PpuConnectCartridge(Cartridge *cartridge);
void PpuClock();


#endif
