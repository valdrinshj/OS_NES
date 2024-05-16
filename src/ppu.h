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



typedef union {
    struct {
        uint8_t unused : 5;
        uint8_t sprite_overflow : 1;
        uint8_t sprite_zero_hit : 1;
        uint8_t vertical_blank : 1;
    } bits;
    uint8_t reg;
}PpuStatus;

typedef union {
    struct {
        uint8_t grayscale : 1;
        uint8_t renderBackgroundLeft : 1;
        uint8_t renderSpritesLeft : 1;
        uint8_t renderBackground : 1;
        uint8_t renderSprites : 1;
        uint8_t enhanceRed : 1;
        uint8_t enhanceGreen : 1;
        uint8_t enhanceBlue : 1;
    } bits;
    uint8_t reg;
} PpuMask;

typedef union {
    struct {
        uint8_t nametable_x : 1;
        uint8_t nametable_y : 1;
        uint8_t increment_mode : 1;
        uint8_t pattern_sprite : 1;
        uint8_t pattern_background : 1;
        uint8_t sprite_size : 1;
        uint8_t slave_mode : 1; // damn, is unused
        uint8_t enable_nmi : 1;
    } bits;
    uint8_t reg;
}PpuControl;

typedef struct {
    PpuControl control;
    PpuStatus status;
    PpuMask mask;
} PpuRegisters;

typedef struct {
    Cartridge *cartridge;
    uint8_t nameTable[2][1024];  // Vram that holds nametable information, 2 * 1 Kb nametables
    uint8_t paletteTable[32]; //Palette information for colors
    uint8_t patternTable[2][4096]; // 2 * 4 KB arrays that represent pattern memory, now unnecessary will only be used if we implement our own mapper
    Color paletteScreen[64];
    Sprite *sprScreen;
    Sprite *sprNameTable[2];
    Sprite *sprPatternTable[2];
    int16_t scanline; // variable to track scanline
    int16_t cycle; // variable to track cycle
    bool frame_complete; // variable to track frame completion
    PpuRegisters registers;
    uint8_t addressLatch;
    uint8_t ppuDataBuffer;
    uint16_t ppu_address;
    bool nmi;
}Ppu;
void ppuInit();

Sprite *spriteCreate(uint16_t width, uint16_t height);
bool spriteSetPixel(Sprite *sprite, uint16_t x, uint16_t y, Color color);

Ppu *ppu_get();
Color spriteGetPixel(Sprite *sprite,uint16_t x, uint16_t y);
Sprite *getPatternTable(uint8_t i, uint8_t palette);
Color getColourFromPaletteRam(uint8_t palette, uint8_t pixel);
//Communication with main bus
uint8_t cpuReadFromPpu(uint16_t addr, bool bReadOnly);
void cpuWriteToPpu(uint16_t addr, uint8_t data);

//Communication with ppu bus
uint8_t ppuRead(uint16_t addr);
void ppuWrite(uint16_t addr, uint8_t data);

// Connect Cartridge to PPU and a clock function
void PpuConnectCartridge(Cartridge *cartridge);
void PpuClock();


#endif
