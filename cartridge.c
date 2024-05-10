#include <stdio.h>
#include <stdlib.h>
#include "cartridge.h"
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

Cartridge *CartridgeCreate(const char *fileName) {
    FILE *fd = fopen(fileName, "rb"); //opens rom in binary mode for reading
    Header header = {0};
    Mapper000 *mapper = (Mapper000*)malloc(sizeof(Mapper000));
    size_t bytes = fread(&header, sizeof(header), 1, fd); // read file header
    if(header.mapper1 & 0x04) {
        fseek(fd, 512, SEEK_CUR);   // 512 bytes of training information should be skipped
    }
    Cartridge *cartridge = (Cartridge*)malloc(sizeof(Cartridge));

    // Determine Mapper ID
    cartridge->nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);

    // we only consider file type 1 right now
    uint8_t nFileType = 1;

    if (nFileType == 0) {

    }
    if (nFileType == 1) {
        cartridge-> nPRGBanks = header.prg_rom_chunks; // read in how many baks of memory are in the rom for the program memory
        cartridge-> PRGMemory = realloc(cartridge -> PRGMemory, 16834); // single bank is 16KB (of program memory)
        bytes = fread(cartridge-> PRGMemory->items, sizeof(uint8_t), sizeof(cartridge->PRGMemory),fd);
        cartridge-> nCHRBanks = header.chr_rom_chunks; // read in how many baks of memory are in the rom for the program memory
        cartridge-> CHRMemory = realloc(cartridge -> CHRMemory, 8192); // single bank is 8KB (of character memory)
        bytes = fread(cartridge-> CHRMemory->items, sizeof(uint8_t), sizeof(cartridge->CHRMemory),fd);
    }
    if (nFileType == 2) {

    }
    // load appropriate mapper, according to mapperID
    switch (cartridge->nMapperID) {
        case 0:  ; //TODO: should work properly
    }
    return cartridge;
    //closed the file
    fclose(fd);
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

