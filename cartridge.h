#ifndef PROJECT_CARTRIDGE_H
#define PROJECT_CARTRIDGE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t *items;
    size_t size;
    size_t capacity;
} Vector; // just like a dynamic array

typedef struct {
    Vector *PRGMemory;
    Vector *CHRMemory;
    uint8_t nMapperID;
    uint8_t nPRGBanks; //sections of ROM --> different section's of the game code and data are divided into banks
    uint8_t nCHRBanks; // the same for CHRBanks, but here graphics data is divided

}Cartridge;

Cartridge *CartridgeCreate(const char *fileName);


//Communication with main bus
bool cartridgeCpuRead(uint16_t addr, uint8_t *data);
bool cartridgeCpuWrite(uint16_t addr, uint8_t data);
//Communication with Ppu bus
bool cartridgePpuRead(uint16_t addr, uint8_t *data);
bool cartridgePpuWrite(uint16_t addr, uint8_t data);


#endif //PROJECT_CARTRIDGE_H
