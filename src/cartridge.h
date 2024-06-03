#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "mapper000.h"
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *items;
    size_t size;
    size_t capacity;
} Vector;

typedef enum {
    HORIZONTAL = 0,
    VERTICAL
} Mirror;

typedef struct {
    Vector *PRGMemory;
    Vector *CHRMemory;
    uint8_t mapperID;
    uint8_t nPRGBanks;
    uint8_t nCHRBanks;
    Mapper *mapper;
    Mirror mirror;
} Cartridge;

Cartridge *CartridgeCreate(const char *romPath);

bool CartridgeCpuRead(Cartridge *cartridge, uint16_t addr, uint8_t * data);
bool CartridgeCpuWrite(Cartridge *cartridge, uint16_t addr, uint8_t data);

bool CartridgePpuRead(Cartridge *cartridge, uint16_t addr, uint8_t * data);
bool CartridgePpuWrite(Cartridge *cartridge, uint16_t addr, uint8_t data);

#endif  // CARTRIDGE_H