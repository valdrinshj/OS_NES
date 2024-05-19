#include <stdlib.h>
#include "mapper000.h"

Mapper000 *MapperCreate(uint8_t PRGbanks, uint8_t CHRbanks) {
    Mapper000 *mapper = (Mapper000*)malloc(sizeof(Mapper000));
    mapper->PRGBanks = PRGbanks;
    mapper->CHRBanks = CHRbanks;
    mapper->cpuMapRead = NULL;
    mapper->cpuMapWrite= NULL;
    mapper->ppuMapRead = NULL;
    mapper->ppuMapWrite = NULL;
    return mapper;
}
