#include <stdio.h>
#include <stdlib.h>
#include "cartridge.h"

#include <assert.h>

#include "mapper000.h"


typedef struct {
    char name[4]; // Identifier for the NES ROM
    uint8_t prg_rom_chunks; // Number of 16 KB PRG-ROM banks
    uint8_t chr_rom_chunks; // Number of 8 KB CHR-ROM banks
    uint8_t mapper1; // Mapper (lower bits)
    uint8_t mapper2; // Mapper(upper bits)
    uint8_t prg_ram_size; // Size of PRG RAM in 8 KB units
    uint8_t tvSystem1;  // TV system compatibility (lower bits)
    uint8_t tvSystem2; // TV system compatibility (upper bits)
    char unused[5]; // Unused bytes
} Header; // provides essential information about the ROM file to the emulator


Vector *VectorCreate(size_t initialCapacity) {
    Vector *vector = (Vector*)malloc(sizeof(Vector));
    if (!vector) {
        fprintf(stderr, "Could't allocate space for the vector\n");
        exit(1);
    }
    vector->items = (uint8_t*)malloc(initialCapacity*sizeof(uint8_t));
    if (!vector->items) {
        fprintf(stderr, "Could't allocate space for the items of the vector\n");
        exit(1);
    }
    vector->size = 0;
    vector->capacity = initialCapacity;
    return vector;
}

Cartridge *CartridgeCreate(const char *fileName) {
    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    cartridge->bImageValid = false;

    FILE *file = fopen(fileName, "rb");
    if (file != NULL) {
        Header header;
        fread(&header, sizeof(Header), 1, file);

        if (header.mapper1 & 0x04)
            fseek(file, 512, SEEK_CUR);

        cartridge->nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
        cartridge->mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

        uint8_t nFileType = 1;

        if (nFileType == 0) {
            // Handle file type 0
        }

        if (nFileType == 1) {
            cartridge->nPRGBanks = header.prg_rom_chunks;
            cartridge->PRGMemory = VectorCreate(16384*header.prg_rom_chunks);
            fread(cartridge->PRGMemory->items, 16384, cartridge->nPRGBanks, file);

            cartridge->nCHRBanks = header.chr_rom_chunks;
            cartridge->CHRMemory = VectorCreate(8192*header.chr_rom_chunks);
            fread(cartridge->CHRMemory->items, 8192, cartridge->nCHRBanks, file);
        }

        if (nFileType == 2) {
            // Handle file type 2
        }

        switch (cartridge->nMapperID) {
            case 0: MapperROM(cartridge->mapper000); break;
            default: assert(cartridge->nMapperID && "Mapper Id not implemented"); break;
        }

        cartridge->bImageValid = true;
        fclose(file);
    }
}
bool cartridgeCpuRead(uint16_t addr, uint8_t *data) {
    uint32_t mapped_address = 0;
    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    Mapper000 *pMapper = cartridge->mapper000;
    if(pMapper->cpuMapRead(pMapper, addr, &mapped_address)) {
        *data = cartridge->PRGMemory->items[mapped_address];
        return true;
    }
    return false;
}
bool cartridgeCpuWrite(uint16_t addr, uint8_t data) {
    uint32_t mapped_address = 0;
    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    Mapper000 *pMapper = cartridge->mapper000;
    if(pMapper->cpuMapWrite(pMapper, &mapped_address)) {
        cartridge->PRGMemory->items[mapped_address] = data;
        return true;
    }
    return false;
}
bool cartridgePpuRead(uint16_t addr, uint8_t *data) {
    uint32_t mapped_address = 0;
    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    Mapper000 *pMapper = cartridge->mapper000;
    if(pMapper->ppuMapRead(pMapper, &mapped_address)) {
        *data = cartridge->CHRMemory->items[mapped_address];
        return true;
    }
    return false;
}
bool cartridgePpuWrite(uint16_t addr, uint8_t data) {
    uint32_t mapped_address = 0;
    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    Mapper000 *pMapper = cartridge->mapper000;
    if(pMapper->ppuMapWrite(pMapper, &mapped_address)) {
        cartridge->CHRMemory->items[mapped_address] = data;
        return true;
    }
    return false;
}

