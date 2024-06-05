#ifndef PPU_H
#define PPU_H


#include "cartridge.h"
#include <raylib.h>
#include <stdint.h>

typedef struct {
    Color *pixels;
    int32_t width;
    int32_t height;
} Sprite;

Sprite *SpriteCreate(int32_t width, int32_t height);
Color SpriteGetPixel(Sprite *sprite, int32_t x, int32_t y);
bool SpriteSetPixel(Sprite *sprite, int32_t x, int32_t y, Color color);

typedef union {
    struct {
        uint8_t nametableX : 1;
        uint8_t nametableY : 1;
        uint8_t incrementMode : 1;
        uint8_t patternSprite : 1;
        uint8_t patternBackground : 1;
        uint8_t spriteSize : 1;
        uint8_t slaveMode : 1;   // unused
        uint8_t enableNmi : 1;
    } bits;
    uint8_t reg;
} PpuCtrl;

typedef union {
    struct {
        uint8_t unused : 5;
        uint8_t spriteOverflow : 1;
        uint8_t spriteZeroHit : 1;
        uint8_t verticalBlank : 1;
    } bits;
    uint8_t reg;
} PpuStatus;

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

typedef struct {
    PpuCtrl ctrl;
    PpuStatus status;
    PpuMask mask;
} PpuRegisters;

typedef union {
    struct {
        uint16_t coarseX : 5;
        uint16_t coarseY : 5;
        uint16_t nametableX : 1;
        uint16_t nametableY : 1;
        uint16_t fineY : 3;
        uint16_t unused : 1;
    } bits;
    uint16_t reg;
} LoopyRegister;

typedef struct {
    Cartridge *cartridge;
    uint8_t nameTable[2][1024];
    uint8_t paletteTable[32];
    uint8_t patternTable[2][4096];   // This table wont be used in the real emulation. Just keep it here for the moment for the design.
    Color paletteScreen[64];
    Sprite *spriteScreen;
    Sprite *spriteNameTable[2];
    Sprite *spritePatternTable[2];
    int16_t scanline;
    int16_t cycle;
    bool frameCompleted;
    PpuRegisters registers;
    LoopyRegister vramAddr;
    LoopyRegister tramAddr;
    uint8_t addressLatch;
    uint8_t ppuDataBuffer;
    uint8_t fineX;
    uint8_t bgNextTileId;
    uint8_t bgNextTileAttr;
    uint8_t bgNextTileLsb;
    uint8_t bgNextTileMsb;
    uint16_t bgShifterPatternLo;
    uint16_t bgShifterPatternHi;
    uint16_t bgShifterAttribLo;
    uint16_t bgShifterAttribHi;
    bool nmi;
    uint8_t *pOAM;
    uint8_t oamAddress;
} Ppu2C02;

typedef struct {
    uint8_t y;			// Y position of sprite
    uint8_t id;			// ID of tile from pattern memory
    uint8_t attribute;	// Flags define how sprite should be rendered
    uint8_t x;			// X position of sprite

} sObjectAttributeEntry;


void PpuInit();

uint8_t CpuReadFromPpu(uint16_t addr, bool readOnly);
void CpuWriteToPpu(uint16_t addr, uint8_t data);

uint8_t PpuRead(uint16_t addr);
void PpuWrite(uint16_t addr, uint8_t data);

Ppu2C02 *PpuGet();

void PpuConnectCartridge(Cartridge *cartridge);
void PpuClock();

Color GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);
Sprite *GetPatternTable(uint8_t i, uint8_t palette);

uint8_t flipbyte(uint8_t b);

#endif  // PPU_H