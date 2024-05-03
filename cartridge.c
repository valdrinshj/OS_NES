#include <stdio.h>
#include <stdlib.h>
#include "cartridge.h"

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

    }
    if (nFileType == 2) {

    }

}