#include "ppu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Color ColorBuild(uint8_t r, uint8_t g, uint8_t b) {
    return (Color){r,g,b,255};
}

static Ppu2C02 ppu = {0};
static PpuCtrl control = {0};
static PpuStatus status = {0};
static sObjectAttributeEntry OAM[64];
static sObjectAttributeEntry spriteScanline[8];


static uint8_t sprite_count;
static uint8_t sprite_shifter_pattern_lo[8];
static uint8_t sprite_shifter_pattern_hi[8];

void PpuInit() {
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

    ppu.spriteScreen = SpriteCreate(256, 240);
    ppu.spriteNameTable[0] = SpriteCreate(256, 240);
    ppu.spriteNameTable[1] = SpriteCreate(256, 240);
    ppu.spritePatternTable[0] = SpriteCreate(128, 128);
    ppu.spritePatternTable[1] = SpriteCreate(128, 128);
    ppu.scanline = 0;
    ppu.cycle = 0;
    ppu.frameCompleted = false;

    ppu.registers.ctrl = (PpuCtrl){0};
    ppu.registers.status = (PpuStatus){0};
    ppu.registers.mask = (PpuMask){0};

    ppu.addressLatch = 0x00;
    ppu.ppuDataBuffer = 0x00;
    ppu.vramAddr = (LoopyRegister){0};
    ppu.vramAddr.reg = 0x0000;
    ppu.tramAddr = (LoopyRegister){0};
    ppu.tramAddr.reg = 0x0000;
    ppu.fineX = 0x00;

    ppu.bgNextTileId = 0x00;
    ppu.bgNextTileAttr = 0x00;
    ppu.bgNextTileLsb = 0x00;
    ppu.bgNextTileMsb = 0x00;

    ppu.bgShifterPatternLo = 0x0000;
    ppu.bgShifterPatternHi = 0x0000;
    ppu.bgShifterAttribLo = 0x0000;
    ppu.bgShifterAttribHi = 0x0000;

    ppu.nmi = false;

    ppu.pOAM = (uint8_t*)OAM;
    ppu.oamAddress = 0x00;
}

Sprite *SpriteCreate(uint16_t width, uint16_t height) {
    Sprite *sprite = (Sprite*)malloc(sizeof(sprite));
    sprite->width = width;
    sprite->height = height;
    sprite->pixels = (Color*)malloc(width*height*sizeof(Color));
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            SpriteSetPixel(sprite, i, j, BLACK);
        }
    }
    return sprite;
}
uint8_t flipbyte(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}


Color SpriteGetPixel(Sprite *sprite, uint16_t x, uint16_t y) {
    return sprite->pixels[x*sprite->height + y];
}

bool SpriteSetPixel(Sprite *sprite, uint16_t x, uint16_t y, Color color) {
    if (x >= 0 && x < sprite->width && y >= 0 && y < sprite->height) {
        sprite->pixels[x*sprite->height + y] = color;
        return true;
    }
    return false;
}

uint8_t CpuReadFromPpu(uint16_t addr, bool readOnly) {
    uint8_t data = 0x00;
    addr &= 0x0007; // PPU is mirrored to the first 8 addresess
    if (readOnly) {
        switch (addr) {
            case 0x0000: // Control
                data = ppu.registers.ctrl.reg;
                break;
            case 0x0001: // Mask
                data = ppu.registers.mask.reg;
                break;
            case 0x0002: // Status
                data = ppu.registers.status.reg;
                break;
            case 0x0003: // OAM Address
                break;
            case 0x0004: // OAM Data
                data = ppu.pOAM[ppu.oamAddress];
                break;
            case 0x0005: // Scroll
                break;
            case 0x0006: // PPU Address
                break;
            case 0x0007: // PPU Data
                break;
        }
    }
    else {
        switch (addr) {

            // Control - Not readable
            case 0x0000: break;

                // Mask - Not Readable
            case 0x0001: break;

                // Status
            case 0x0002:
                data = (ppu.registers.status.reg & 0xE0) | (ppu.ppuDataBuffer & 0x1F);
                ppu.registers.status.bits.verticalBlank = 0;
                ppu.addressLatch = 0;
                break;

                // OAM Address
            case 0x0003: break;

                // OAM Data
            case 0x0004: break;

                // Scroll - Not Readable
            case 0x0005: break;

                // PPU Address - Not Readable
            case 0x0006: break;

                // PPU Data
            case 0x0007:
                data = ppu.ppuDataBuffer;
                ppu.ppuDataBuffer = PpuRead(ppu.vramAddr.reg);
                if (ppu.vramAddr.reg >= 0x3F00) data = ppu.ppuDataBuffer;
                ppu.vramAddr.reg += ppu.registers.ctrl.bits.incrementMode ? 32 : 1;
                break;
        }
    }
    return data;
}

void CpuWriteToPpu(uint16_t addr, uint8_t data) {
    addr &= 0x0007; // PPU is mirrored to the first 8 addresess
    switch (addr) {
        case 0x0000: // Control
            ppu.registers.ctrl.reg = data;
            ppu.tramAddr.bits.nametableX = ppu.registers.ctrl.bits.nametableX;
            ppu.tramAddr.bits.nametableY = ppu.registers.ctrl.bits.nametableY;
            break;
        case 0x0001: // Mask
            ppu.registers.mask.reg = data;
            break;
        case 0x0002: // Status
            break;
        case 0x0003: // OAM Address
            ppu.oamAddress = data;
            break;
        case 0x0004: // OAM Data
            ppu.pOAM[ppu.oamAddress] = data;
            break;
        case 0x0005: // Scroll
            if (ppu.addressLatch == 0) {
                ppu.fineX = data & 0x07;
                ppu.tramAddr.bits.coarseX = data >> 3;
                ppu.addressLatch = 1;
            }
            else {
                ppu.tramAddr.bits.fineY = data & 0x07;
                ppu.tramAddr.bits.coarseY = data >> 3;
                ppu.addressLatch = 0;
            }
            break;
        case 0x0006: // PPU Address
            if (ppu.addressLatch == 0) {
                ppu.tramAddr.reg = (uint16_t)((data & 0x3F) << 8) | (ppu.tramAddr.reg & 0x00FF);
                ppu.addressLatch = 1;
            }
            else {
                ppu.tramAddr.reg = (ppu.tramAddr.reg & 0xFF00) | data;
                ppu.vramAddr = ppu.tramAddr;
                ppu.addressLatch = 0;
            }
            break;
        case 0x0007: // PPU Data
            PpuWrite(ppu.vramAddr.reg, data);
            ppu.vramAddr.reg += ppu.registers.ctrl.bits.incrementMode ? 32 : 1;
            break;
    }
}

uint8_t PpuRead(uint16_t addr) {
    uint8_t data = 0x00;
    addr &= 0x3FFF;

    if (CartridgePpuRead(ppu.cartridge, addr, &data)) {

    }
        // Pattern table
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // Get the most significant bit of the address and offsets by the rest of the bits of the address
        data = ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF];
    }
        // Name table
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        if (ppu.cartridge->mirror == VERTICAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF) data = ppu.nameTable[1][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF) data = ppu.nameTable[1][addr & 0x03FF];
        }
        else if (ppu.cartridge->mirror == HORIZONTAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF) data = ppu.nameTable[0][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF) data = ppu.nameTable[1][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF) data = ppu.nameTable[1][addr & 0x03FF];
        }
    }
        // Palette
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F; // Mask less significant 5 bits
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        data = ppu.paletteTable[addr];
    }

    return data;
}

void PpuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;

    if (CartridgePpuWrite(ppu.cartridge, addr, data)) {

    }
        // Pattern table
    else if (addr >= 0x0000 && addr <= 0x1FFF) {
        // This memory acts as a ROM for the PPU,
        // but for som NES ROMs, this memory is a RAM.
        ppu.patternTable[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
    }
        // Name table
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        if (ppu.cartridge->mirror == VERTICAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF) ppu.nameTable[1][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF) ppu.nameTable[1][addr & 0x03FF] = data;
        }
        else if (ppu.cartridge->mirror == HORIZONTAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF) ppu.nameTable[0][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF) ppu.nameTable[1][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF) ppu.nameTable[1][addr & 0x03FF] = data;
        }
    }
        // Palette
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F; // Mask less significant 5 bits
        if (addr == 0x0010) addr = 0x0000;
        if (addr == 0x0014) addr = 0x0004;
        if (addr == 0x0018) addr = 0x0008;
        if (addr == 0x001C) addr = 0x000C;
        ppu.paletteTable[addr] = data;
    }
}

Ppu2C02 *PpuGet() {
    return &ppu;
}

void PpuConnectCartridge(Cartridge *cartridge) {
    ppu.cartridge = cartridge;
}

Color GetColourFromPaletteRam(uint8_t palette, uint8_t pixel) {
    return ppu.paletteScreen[PpuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}

Sprite *GetPatternTable(uint8_t i, uint8_t palette) {
    for (uint16_t nTileY = 0; nTileY < 16; nTileY++) {
        for (uint16_t nTileX = 0; nTileX < 16; nTileX++) {
            uint16_t nOffset = nTileY * 256 + nTileX * 16;
            // Now loop through 8 rows of 8 pixels (Tile)
            for (uint16_t row = 0; row < 8; row++) {
                uint8_t tile_lsb = PpuRead(i * 0x1000 + nOffset + row + 0x0000);
                uint8_t tile_msb = PpuRead(i * 0x1000 + nOffset + row + 0x0008);
                for (uint16_t col = 0; col < 8; col++) {
                    uint8_t pixel = ((tile_lsb & 0x01) << 1) | ((tile_msb & 0x01));
                    tile_lsb >>= 1; tile_msb >>= 1;
                    Color c = GetColourFromPaletteRam(palette, pixel);
                    SpriteSetPixel(ppu.spritePatternTable[i], nTileX * 8 + (7 - col), nTileY * 8 + row, c);
                }
            }
        }
    }
    return ppu.spritePatternTable[i];
}

void PpuIncrementScrollX() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        if (ppu.vramAddr.bits.coarseX == 31) {
            ppu.vramAddr.bits.coarseX = 0;
            ppu.vramAddr.bits.nametableX = ~ppu.vramAddr.bits.nametableX;
        }
        else {
            ppu.vramAddr.bits.coarseX++;
        }
    }
}

void PpuIncrementScrollY() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        if (ppu.vramAddr.bits.fineY < 7) {
            ppu.vramAddr.bits.fineY++;
        }
        else {
            ppu.vramAddr.bits.fineY = 0;
            if (ppu.vramAddr.bits.coarseY == 29) {
                ppu.vramAddr.bits.coarseY = 0;
                ppu.vramAddr.bits.nametableY = ~ppu.vramAddr.bits.nametableY;
            }
            else if (ppu.vramAddr.bits.coarseY == 31) {
                ppu.vramAddr.bits.coarseY = 0;
            }
            else {
                ppu.vramAddr.bits.coarseY++;
            }
        }
    }
}

void PpuTransferAddressX() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        ppu.vramAddr.bits.nametableX = ppu.tramAddr.bits.nametableX;
        ppu.vramAddr.bits.coarseX = ppu.tramAddr.bits.coarseX;
    }
}

void PpuTransferAddressY() {
    if (ppu.registers.mask.bits.renderBackground || ppu.registers.mask.bits.renderSprites) {
        ppu.vramAddr.bits.fineY = ppu.tramAddr.bits.fineY;
        ppu.vramAddr.bits.nametableY = ppu.tramAddr.bits.nametableY;
        ppu.vramAddr.bits.coarseY = ppu.tramAddr.bits.coarseY;
    }
}

void PpuLoadBackgroundShifters() {
    ppu.bgShifterPatternLo = (ppu.bgShifterPatternLo & 0xFF00) | ppu.bgNextTileLsb;
    ppu.bgShifterPatternHi = (ppu.bgShifterPatternHi & 0xFF00) | ppu.bgNextTileMsb;
    ppu.bgShifterAttribLo = (ppu.bgShifterAttribLo & 0xFF00) | ((ppu.bgNextTileAttr & 0b01) ? 0xFF : 0x00);
    ppu.bgShifterAttribHi = (ppu.bgShifterAttribHi & 0xFF00) | ((ppu.bgNextTileAttr & 0b10) ? 0xFF : 0x00);
}

void PpuUpdateShifters() {
    if (ppu.registers.mask.bits.renderBackground) {
        ppu.bgShifterPatternLo <<= 1;
        ppu.bgShifterPatternHi <<= 1;
        ppu.bgShifterAttribLo <<= 1;
        ppu.bgShifterAttribHi <<= 1;
    }
    if(ppu.registers.mask.bits.renderSprites && ppu.cycle >= 1 && ppu.cycle < 258) {
        for(int i = 0; i < sprite_count; i++) {
            if(spriteScanline[i].x > 0) {
                spriteScanline[i].x--;
            } else {
                sprite_shifter_pattern_lo[i] << 1;
                sprite_shifter_pattern_hi[i] << 1;
            }
        }
    }
}

void PpuClock() {
    if (ppu.scanline >= -1 && ppu.scanline < 240) {
        if (ppu.scanline == 0 && ppu.cycle == 0) {
            ppu.cycle = 1;
        }
        if (ppu.scanline == -1 && ppu.cycle == 1) {
            ppu.registers.status.bits.verticalBlank = 0;
            status.bits.spriteOverflow = 0;
            for(int i = 0; i < 8; i++) {
                sprite_shifter_pattern_lo[i] = 0;
                sprite_shifter_pattern_hi[i] = 0;
            }
        }
        if ((ppu.cycle >= 2 && ppu.cycle < 258) || (ppu.cycle >= 321 && ppu.cycle < 338)) {
            PpuUpdateShifters();
            switch ((ppu.cycle - 1) % 8) {
                case 0:
                    PpuLoadBackgroundShifters();
                    ppu.bgNextTileId = PpuRead(0x2000 | (ppu.vramAddr.reg & 0x0FFF));
                    break;
                case 2:
                    ppu.bgNextTileAttr = PpuRead(0x23C0 | (ppu.vramAddr.bits.nametableY << 11)
                                                 | (ppu.vramAddr.bits.nametableX << 10)
                                                 | ((ppu.vramAddr.bits.coarseY >> 2) << 3)
                                                 | (ppu.vramAddr.bits.coarseX >> 2));
                    if (ppu.vramAddr.bits.coarseY & 0x02) ppu.bgNextTileAttr >>= 4;
                    if (ppu.vramAddr.bits.coarseX & 0x02) ppu.bgNextTileAttr >>= 2;
                    ppu.bgNextTileAttr &= 0x03;
                    break;
                case 4:
                    ppu.bgNextTileLsb = PpuRead((ppu.registers.ctrl.bits.patternBackground << 12)
                                                + ((uint16_t)ppu.bgNextTileId << 4)
                                                + (ppu.vramAddr.bits.fineY) + 0);
                    break;
                case 6:
                    ppu.bgNextTileMsb = PpuRead((ppu.registers.ctrl.bits.patternBackground << 12)
                                                + ((uint16_t)ppu.bgNextTileId << 4)
                                                + (ppu.vramAddr.bits.fineY) + 8);
                    break;
                case 7:
                    PpuIncrementScrollX();
                    break;
                default:
                    break;
            }
        }
        if (ppu.cycle == 256) {
            PpuIncrementScrollY();
        }
        if (ppu.cycle == 257) {
            PpuLoadBackgroundShifters();
            PpuTransferAddressX();
        }
        if (ppu.cycle == 338 || ppu.cycle == 340) {
            ppu.bgNextTileId = PpuRead(0x2000 | (ppu.vramAddr.reg & 0x0FFF));
        }
        if (ppu.scanline == -1 && ppu.cycle >= 280 && ppu.cycle < 305) {
            PpuTransferAddressY();
        }

        // FOREGROUND RENDERING
        if (ppu.cycle == 257 && ppu.scanline > 0) {
            memset(spriteScanline,0xFF, 8 * sizeof(spriteScanline));
            sprite_count = 0;

            uint8_t nOAMEntry = 0;
            while (nOAMEntry < 64 && sprite_count < 9) {
                int16_t diff = ((int16_t)ppu.scanline - (int16_t)OAM[nOAMEntry].y);
                if(diff >= 0 && diff < control.bits.spriteSize ? 16 : 8) {
                    if(sprite_count < 8) {
                        memcpy(&spriteScanline, &OAM[nOAMEntry], sizeof(spriteScanline));
                        sprite_count++;
                    }
                }
                nOAMEntry++;
            }
            status.bits.spriteOverflow = (sprite_count > 8);
        }
        if (ppu.cycle == 340) {
            for (uint8_t i = 0; i < sprite_count; i++) {
                uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
                uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

                if(!control.bits.spriteSize) {
                    // 8x8
                    if(!(spriteScanline->attribute & 0x80)) {
                        // Sprite is NOT flipped vertically
                        sprite_pattern_addr_lo = (control.bits.patternSprite << 12) | (spriteScanline[i].id << 4) | (ppu.scanline - spriteScanline[i].y);
                    } else {
                        // Sprite is flipped
                        sprite_pattern_addr_lo = (control.bits.patternSprite << 12) | (spriteScanline[i].id << 4) | (7 - (ppu.scanline - spriteScanline[i].y));
                    }
                } else {
                    // 8x16
                    if(!(spriteScanline[i].attribute & 0x80)) {
                        // Sprite is NOT flipped vertically
                        if (ppu.scanline - spriteScanline[i].y < 8) {
                            // reading top half tile
                            sprite_pattern_addr_lo = ((spriteScanline[i].id & 0x01) << 12)
                            | (((spriteScanline[i].id & 0xFE)) << 4)
                            | ((ppu.scanline - spriteScanline[i].y) & 0x07);
                        } else {
                            // reading bottom half tile
                            sprite_pattern_addr_lo = ((spriteScanline[i].id & 0x01) << 12)
                            | (((spriteScanline[i].id & 0xFE) + 1) << 4)
                            | ((ppu.scanline - spriteScanline[i].y) & 0x07);
                        }

                    } else {
                        // Sprite is flipped vertically
                        if(ppu.scanline - spriteScanline[i].y < 8) {
                            // reading top half tile
                            sprite_pattern_addr_lo = ((spriteScanline[i].id & 0x01) << 12)
                            | (((spriteScanline[i].id & 0xFE) + 1) << 4)
                            | (7- (ppu.scanline - spriteScanline[i].y) & 0x07);
                        } else {
                            // reading bottom half tile
                            sprite_pattern_addr_lo = ((spriteScanline[i].id & 0x01) << 12)
                            | (((spriteScanline[i].id & 0xFE)) << 4)
                            | (7 - (ppu.scanline - spriteScanline[i].y) & 0x07);

                        }
                    }
                }

                sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;
                sprite_pattern_bits_lo = PpuRead(sprite_pattern_addr_lo);
                sprite_pattern_bits_hi = PpuRead(sprite_pattern_addr_hi);

                if(spriteScanline[i].attribute & 0x40) {

                    sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
                    sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
                }

                sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
                sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
            }
        }
    }

    if (ppu.scanline == 240) {
        // Nothing happen here
    }

    if (ppu.scanline >= 241 && ppu.scanline < 261) {
        if (ppu.scanline == 241 && ppu.cycle == 1) {
            ppu.registers.status.bits.verticalBlank = 1;
            if (ppu.registers.ctrl.bits.enableNmi) {
                ppu.nmi = true;
            }
        }
    }

    uint8_t bgPixel = 0x00;
    uint8_t bgPalette = 0x00;

    if (ppu.registers.mask.bits.renderBackground) {
        uint16_t bitMux = 0x8000 >> ppu.fineX;
        uint8_t p0Pixel = (ppu.bgShifterPatternLo & bitMux) > 0;
        uint8_t p1Pixel = (ppu.bgShifterPatternHi & bitMux) > 0;
        bgPixel = (p1Pixel << 1) | p0Pixel;

        uint8_t bgPalette0 = (ppu.bgShifterAttribLo & bitMux) > 0;
        uint8_t bgPalette1 = (ppu.bgShifterAttribHi & bitMux) > 0;
        bgPalette = (bgPalette1 << 1) | bgPalette0;
    }
    uint8_t fgPixel = 0x00;
    uint8_t fgPalette = 0x00;
    uint8_t fgPriority = 0x00;

    if (ppu.registers.mask.bits.renderSprites) {
        for(uint8_t i = 0; i < sprite_count ; i++) {
            if(spriteScanline[i].x == 0) {
                uint8_t fgPixelLo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
                uint8_t fgPixelHi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
                fgPixel = (fgPixelHi << 1) | fgPixelLo;

                fgPalette = (spriteScanline[i].attribute & 0x03) + 0x04;
                fgPriority = (spriteScanline[i].attribute & 0x20) == 0;
                if(fgPixel != 0) {
                    break;
                }
            }
        }
    }

    uint8_t pixel = 0x00; // Final pixel
    uint8_t palette = 0x00; // Final palette

    if (bgPixel == 0 && fgPixel == 0) {
        // transparent
        pixel = 0x00;
        palette = 0x00;
    } else if(bgPixel == 0 && fgPixel > 0) {
        pixel = fgPixel;
        palette = fgPalette;
    } else if (bgPixel > 0 && fgPixel == 0) {
        pixel = bgPixel;
        palette = bgPalette;
    } else if (bgPixel > 0 && fgPixel > 0) {
        if (fgPriority) {
            pixel = fgPixel;
            palette = fgPalette;
        } else {
            pixel = bgPixel;
            palette = bgPalette;
        }
    }

    // Update the Sprite screen with the appropiate pixels and palettes
    SpriteSetPixel(ppu.spriteScreen, ppu.cycle - 1, ppu.scanline, GetColourFromPaletteRam(palette, pixel));

    // Advance renderer - it never stops, it's relentless
    ppu.cycle++;
    if (ppu.cycle >= 341) {
        ppu.cycle = 0;
        ppu.scanline++;
        if (ppu.scanline >= 261) {
            ppu.scanline = -1;
            ppu.frameCompleted = true;
        }
    }
}