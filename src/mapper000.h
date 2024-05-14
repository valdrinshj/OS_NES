#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Mapper{
    uint8_t PRGBanks;
    uint8_t CHRBanks;
    bool (*cpuMapRead)(struct Mapper *this,uint16_t addr, uint32_t *mapped_addr);
    bool (*cpuMapWrite)(struct Mapper *this, uint32_t *mapped_addr);
    bool (*ppuMapRead)(struct Mapper *this, uint32_t *mapped_addr);
    bool (*ppuMapWrite)(struct Mapper *this, uint32_t *mapped_addr);

} Mapper000;

#endif // MAPPER_H


