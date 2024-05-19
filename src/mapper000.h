#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Mapper{
    uint8_t PRGBanks;
    uint8_t CHRBanks;
    bool (*cpuMapRead)(struct Mapper *this,uint16_t addr, uint32_t *mapped_addr);
    bool (*cpuMapWrite)(struct Mapper *this,uint16_t addr,uint32_t *mapped_addr);
    bool (*ppuMapRead)(struct Mapper *this, uint16_t addr,uint32_t *mapped_addr);
    bool (*ppuMapWrite)(struct Mapper *this, uint16_t addr,uint32_t *mapped_addr);

} Mapper000;

Mapper000 *MapperCreate(uint8_t PRGbanks, uint8_t CHRbanks);

void MapperROM(Mapper000 *mapper);

#endif // MAPPER_H


