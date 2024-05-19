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
    FILE *fd = fopen(fileName, "rb");
    if (fd == NULL) {
        fprintf(stderr, "Error opening '%s' file\n", fileName);
        exit(1);
    }
    Header header = {0};
    size_t bytes = fread(&header, sizeof(Header), 1, fd);
    if (header.mapper1 & 0x04)
        fseek(fd, 512, SEEK_CUR);   // Ignore the 512 bytes of Training information in case that exist

    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));
    if (!cartridge) {
        fprintf(stderr, "Could't allocate space for the cartridge\n");
        exit(1);
    }
    cartridge->PRGMemory = VectorCreate(16384*header.prg_rom_chunks);
    cartridge->CHRMemory = VectorCreate(8192*header.chr_rom_chunks);
    cartridge->nPRGBanks = header.prg_rom_chunks;
    cartridge->nCHRBanks = header.chr_rom_chunks;
    cartridge->nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
    cartridge->mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

    uint8_t fileType = 1;
    // By the moment we only consider file type 1
    if (fileType == 1) {
        printf("PRG_chunks = %d\n", header.prg_rom_chunks);
        printf("CHR_chunks = %d\n", header.chr_rom_chunks);
        bytes = fread(cartridge->PRGMemory->items, sizeof(uint8_t), 16384*header.prg_rom_chunks, fd);
        printf("PRGMemory readed = %d\n", bytes);
        bytes = fread(cartridge->CHRMemory->items, sizeof(uint8_t), 8192*header.chr_rom_chunks, fd);

        printf("CHRMemory readed = %d\n", bytes);
    }
    if (fclose(fd)) {
        fprintf(stdout, "Error closing the file\n");
        exit(1);
    }
    cartridge->mapper000 = MapperCreate(cartridge->nPRGBanks, cartridge->nCHRBanks);
    switch (cartridge->nMapperID) {
        case 0: MapperROM(cartridge->mapper000);
        default: assert(cartridge->nMapperID && "Mapper ID not implemented"); break;
    }
    return cartridge;
}

bool cartridgeCpuRead(Cartridge *cartridge, uint16_t addr, uint8_t *data) {
    uint32_t mapped_address = 0;
    Mapper000 *mapper = cartridge->mapper000;
    if(mapper->cpuMapRead(mapper, addr, &mapped_address)) {
        *data = cartridge->PRGMemory->items[mapped_address];
        return true;
    }
    return false;
}
bool cartridgeCpuWrite(Cartridge *cartridge,uint16_t addr, uint8_t data) {
    uint32_t mapped_address = 0;
    Mapper000 *mapper = cartridge->mapper000;
    if(mapper->cpuMapWrite(mapper, addr,&mapped_address)) {
        cartridge->PRGMemory->items[mapped_address] = data;
        return true;
    }
    return false;
}
bool cartridgePpuRead(Cartridge *cartridge, uint16_t addr, uint8_t* data) {
    uint32_t mapped_address = 0;
    Mapper000 *mapper = cartridge->mapper000;
    if(mapper->ppuMapRead(mapper, addr,&mapped_address)) {
        *data = cartridge->CHRMemory->items[mapped_address];
        return true;
    }
    return false;
}
bool cartridgePpuWrite(Cartridge *cartridge,uint16_t addr, uint8_t data) {
    uint32_t mapped_address = 0;
    Mapper000 *mapper = cartridge->mapper000;
    if(mapper->ppuMapWrite(mapper, addr,&mapped_address)) {
        cartridge->CHRMemory->items[mapped_address] = data;
        return true;
    }
    return false;
}

