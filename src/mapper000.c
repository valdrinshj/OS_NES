#include "mapper000.h"

Mapper *MapperCreate(uint8_t PRGbanks, uint8_t CHRbanks) {
    Mapper *mapper = (Mapper*)malloc(sizeof(Mapper));
    mapper->PRGbanks = PRGbanks;
    mapper->CHRbanks = CHRbanks;
    mapper->MapperCpuRead = NULL;
    mapper->MapperCpuWrite = NULL;
    mapper->MapperPpuRead = NULL;
    mapper->MapperPpuWrite = NULL;
    return mapper;
}

bool MapperCpuRead(Mapper *mapper, uint16_t addr, uint32_t * mappedAddr);
bool MapperCpuWrite(Mapper *mapper, uint16_t addr, uint32_t* mappedAddr);

bool MapperPpuRead(Mapper *mapper, uint16_t addr, uint32_t* mappedAddr);
bool MapperPpuWrite(Mapper *mapper, uint16_t addr, uint32_t* mappedAddr);

void MapperLoadNROM(Mapper *mapper) {
    mapper->MapperCpuRead = MapperCpuRead;
    mapper->MapperCpuWrite = MapperCpuWrite;
    mapper->MapperPpuRead = MapperPpuRead;
    mapper->MapperPpuWrite = MapperPpuWrite;
}

bool MapperCpuRead(Mapper *mapper, uint16_t addr, uint32_t* mappedAddr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        *mappedAddr = addr & (mapper->PRGbanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool MapperCpuWrite(Mapper *mapper, uint16_t addr, uint32_t* mappedAddr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        *mappedAddr = addr & (mapper->PRGbanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }
    return false;
}

bool MapperPpuRead(Mapper *mapper, uint16_t addr, uint32_t* mappedAddr) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        *mappedAddr = addr;
        return true;
    }
    return false;
}

bool MapperPpuWrite(Mapper *mapper, uint16_t addr, uint32_t* mappedAddr) {
    // if (addr >= 0x0000 && addr <= 0x1FFF) {
    //     if (mapper->CHRbanks == 0) {
    //         // Treat as RAM
    //         *mappedAddr = addr;
    //         return true;
    //     }
    // }
    return false;
}