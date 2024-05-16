#include "bus.h"
#include "ppu.h"

#include <stdlib.h>

#include "raylib.h"

Color ColorBuild(uint8_t r, uint8_t g, uint8_t b) {
    return (Color){r,g,b,255};
}
static Ppu ppu = {0};

void ppuInit() {
    // Array of all the available colors the nes is capable of doing
    ppu.paletteScreen[0x00] = ColorBuild(84, 84, 84);
    ppu.paletteScreen[0x01] = ColorBuild(0, 30, 116);
    ppu.paletteScreen[0x02] = ColorBuild(8, 16, 144);
    ppu.paletteScreen[0x03] = ColorBuild(48, 0, 136);
    ppu.paletteScreen[0x04] = ColorBuild(68, 0, 100);
    ppu.paletteScreen[0x05] = ColorBuild(92, 0, 48);
    ppu.paletteScreen[0x06] = ColorBuild(84, 4, 0);
    ppu.paletteScreen[0x07] = ColorBuild(60, 24, 0);
    ppu.paletteScreen[0x08] = ColorBuild(32, 42, 0);
    ppu.paletteScreen[0x09] = ColorBuild(8, 58, 0);
    ppu.paletteScreen[0x0A] = ColorBuild(0, 64, 0);
    ppu.paletteScreen[0x0B] = ColorBuild(0, 60, 0);
    ppu.paletteScreen[0x0C] = ColorBuild(0, 50, 60);
    ppu.paletteScreen[0x0D] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x0E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x0F] = ColorBuild(0, 0, 0);

    ppu.paletteScreen[0x10] = ColorBuild(152, 150, 152);
    ppu.paletteScreen[0x11] = ColorBuild(8, 76, 196);
    ppu.paletteScreen[0x12] = ColorBuild(48, 50, 236);
    ppu.paletteScreen[0x13] = ColorBuild(92, 30, 228);
    ppu.paletteScreen[0x14] = ColorBuild(136, 20, 176);
    ppu.paletteScreen[0x15] = ColorBuild(160, 20, 100);
    ppu.paletteScreen[0x16] = ColorBuild(152, 34, 32);
    ppu.paletteScreen[0x17] = ColorBuild(120, 60, 0);
    ppu.paletteScreen[0x18] = ColorBuild(84, 90, 0);
    ppu.paletteScreen[0x19] = ColorBuild(40, 114, 0);
    ppu.paletteScreen[0x1A] = ColorBuild(8, 124, 0);
    ppu.paletteScreen[0x1B] = ColorBuild(0, 118, 40);
    ppu.paletteScreen[0x1C] = ColorBuild(0, 102, 120);
    ppu.paletteScreen[0x1D] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x1E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x1F] = ColorBuild(0, 0, 0);

    ppu.paletteScreen[0x20] = ColorBuild(236, 238, 236);
    ppu.paletteScreen[0x21] = ColorBuild(76, 154, 236);
    ppu.paletteScreen[0x22] = ColorBuild(120, 124, 236);
    ppu.paletteScreen[0x23] = ColorBuild(176, 98, 236);
    ppu.paletteScreen[0x24] = ColorBuild(228, 84, 236);
    ppu.paletteScreen[0x25] = ColorBuild(236, 88, 180);
    ppu.paletteScreen[0x26] = ColorBuild(236, 106, 100);
    ppu.paletteScreen[0x27] = ColorBuild(212, 136, 32);
    ppu.paletteScreen[0x28] = ColorBuild(160, 170, 0);
    ppu.paletteScreen[0x29] = ColorBuild(116, 196, 0);
    ppu.paletteScreen[0x2A] = ColorBuild(76, 208, 32);
    ppu.paletteScreen[0x2B] = ColorBuild(56, 204, 108);
    ppu.paletteScreen[0x2C] = ColorBuild(56, 180, 204);
    ppu.paletteScreen[0x2D] = ColorBuild(60, 60, 60);
    ppu.paletteScreen[0x2E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x2F] = ColorBuild(0, 0, 0);

    ppu.paletteScreen[0x30] = ColorBuild(236, 238, 236);
    ppu.paletteScreen[0x31] = ColorBuild(168, 204, 236);
    ppu.paletteScreen[0x32] = ColorBuild(188, 188, 236);
    ppu.paletteScreen[0x33] = ColorBuild(212, 178, 236);
    ppu.paletteScreen[0x34] = ColorBuild(236, 174, 236);
    ppu.paletteScreen[0x35] = ColorBuild(236, 174, 212);
    ppu.paletteScreen[0x36] = ColorBuild(236, 180, 176);
    ppu.paletteScreen[0x37] = ColorBuild(228, 196, 144);
    ppu.paletteScreen[0x38] = ColorBuild(204, 210, 120);
    ppu.paletteScreen[0x39] = ColorBuild(180, 222, 120);
    ppu.paletteScreen[0x3A] = ColorBuild(168, 226, 144);
    ppu.paletteScreen[0x3B] = ColorBuild(152, 226, 180);
    ppu.paletteScreen[0x3C] = ColorBuild(160, 214, 228);
    ppu.paletteScreen[0x3D] = ColorBuild(160, 162, 160);
    ppu.paletteScreen[0x3E] = ColorBuild(0, 0, 0);
    ppu.paletteScreen[0x3F] = ColorBuild(0, 0, 0);

    ppu.sprScreen = spriteCreate(256,240);
    ppu.sprNameTable[0] = spriteCreate(256,240);
    ppu.sprNameTable[1] = spriteCreate(256,240);
    // will be used to visualize the pattern memory
    ppu.sprPatternTable[0] = spriteCreate(128, 128);
    ppu.sprPatternTable[1] = spriteCreate(128, 128);
}
// functtion to create a new Sprite
Sprite *spriteCreate(uint16_t width, uint16_t height) {
    Sprite *sprite = (Sprite*)malloc(sizeof(sprite));
    if(sprite == NULL) {
        free(sprite);
        return NULL;
    }
    sprite->width = width;
    sprite->height = height;
    sprite->pixel = (Color*)malloc(width * height * sizeof(Color));
    if(sprite->pixel == NULL) {
        free(sprite);
        return NULL;
    }
    return sprite;
}
Ppu *ppu_get() {
    return &ppu;
}
Color spriteGetPixel(Sprite *sprite,uint16_t x, uint16_t y) {
        return sprite->pixel[x*sprite->height + y];
}
// Function to set a pixel in the Sprite
bool spriteSetPixel(Sprite *sprite, uint16_t x, uint16_t y, Color color) {
    //Check if the coordinates are within the bounds
    if(x >= 0 && x < sprite->width && y >= 0 && y < sprite->height) {
        //set the color of the pixel
        sprite->pixel[x*sprite->height + y] = color;
        return true;
    }
    return false;
}

Sprite *getPatternTable(uint8_t i, uint8_t palette) {
    // because patternTable consists of 16x16 Tiles
    for(uint16_t nTileY = 0; nTileY < 16; nTileY++) {
        for(uint16_t nTileX = 0; nTileX < 16; nTileX++) {
            // heigth and width of tile
            uint16_t nOffset = nTileY * 256 + nTileX * 16;
            // 8 rows and columns with each 8 pixels
            for (uint16_t row = 0; row < 8; row++) {
                    uint8_t tile_lsb = ppuRead(i * 0x1000 + nOffset + row + 0);
                    uint8_t tile_msb = ppuRead(i * 0x1000 + nOffset + row + 8);

                for(uint16_t col = 0; col < 8; col++) {
                    // value between 0 and 3
                    uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);
                    tile_lsb >>= 1; tile_msb >>=1;
                    Color c = getColourFromPaletteRam(palette, pixel);
                    spriteSetPixel(ppu.sprPatternTable[i], nTileX * 8 + (7 - col), nTileY * 8 + row, c);
                }
            }
        }
    }
    return ppu.sprPatternTable[i];
}
Color getColourFromPaletteRam(uint8_t palette, uint8_t pixel) {
    return ppu.paletteScreen[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}


uint8_t cpuReadFromPpu(uint16_t addr, bool bReadOnly) {
    uint8_t data = 0x00;

    switch (addr) {
        case 0x0000: // Control
            break;
        case 0x0001: // Mask
            break;
        case 0x0002: // Status
            data = (ppu.registers.status.reg & 0xE0  | (ppu.ppuDataBuffer & 0x1F));
            ppu.registers.status.bits.vertical_blank = 0;
            ppu.addressLatch = 0;
            break;
        case 0x0003: // OAM Address
            break;
        case 0x0004: // OAM Data
            break;
        case 0x0005: // Scroll
            break;
        case 0x0006: // PPU Address cant read
            break;
        case 0x0007: // PPU Data
            data = ppu.ppuDataBuffer;
            ppu.ppuDataBuffer = ppuRead(ppu.ppu_address);
            // not wait until next clock cycle
            if(ppu.ppu_address > 0x3f00) {
                data = ppu.ppuDataBuffer;
            }
            ppu.ppu_address++;
            break;
    }

    return data;
}
void cpuWriteToPpu(uint16_t addr, uint8_t data) {
    switch (addr)
    {
        case 0x0000: // Control
            ppu.registers.control.reg = data;
            break;
        case 0x0001: // Mask
            ppu.registers.mask.reg = data;
            break;
        case 0x0002: // Status -> cant write
            break;
        case 0x0003: // OAM Address
            break;
        case 0x0004: // OAM Data
            break;
        case 0x0005: // Scroll
            break;
        case 0x0006: // PPU Address
            if(ppu.addressLatch == 0) {
                ppu.ppu_address = (ppu.ppu_address & 0xFF00) | data;
                ppu.addressLatch = 1;
            } else {
                ppu.ppu_address = (ppu.ppu_address & 0xFF00) | data;
                ppu.addressLatch = 0;
            }
            break;
        case 0x0007: // PPU Data
            ppuWrite(ppu.ppu_address, data);
            ppu.ppu_address++;
            break;
    }
}

uint8_t ppuRead(uint16_t addr) {
    uint8_t data = 0x00;
    addr &= 0x3FFF;  //placeholders
    if(cartridgePpuRead(addr, &data)) {

    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // read from pattern memory -> examining msb and offsets by the rest of the address
        data = ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF];
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {

    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F; // Mask less significant 5 bits
        // direct mirroring
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        data = ppu.paletteTable[addr]; // read directly from memory location
    }
    return data;
}
void ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;
    if(cartridgePpuWrite(addr, data)) {

    } else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // This memory acts as a ROM for the PPU
        // in some cartridges a RAM
        ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    } else if (addr >= 0x2000 && addr <= 0x3EFF) {

    } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F; // Mask less significant 5 bits
        // direct mirroring
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        ppu.paletteTable[addr] = data;
    }
}
void PpuConnectCartridge(Cartridge *cartridge) {
    ppu.cartridge = cartridge;
}
void PpuClock() {
    if(ppu.scanline == -1 && ppu.cycle == 1) {
        ppu.registers.status.bits.vertical_blank = 0;
    }
    if(ppu.scanline == 241 && ppu.cycle == 1) {
        ppu.registers.status.bits.vertical_blank = 1;
        if(ppu.registers.control.bits.enable_nmi) {
            ppu.nmi = true;
        }
    }



    // fakes some noise for now
    spriteSetPixel(ppu.sprScreen, ppu.cycle - 1, ppu.scanline, ppu.paletteScreen[(rand() % 2) ? 0x3F : 0x30]);
    // renderer
    ppu.cycle++;
    if(ppu.cycle >= 341) {
        ppu.cycle = 0;
        ppu.scanline++;
        if(ppu.scanline >= 261) {
            ppu.scanline--;
            ppu.frame_complete = true;
        }
    }
}
