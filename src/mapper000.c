#include "mapper000.h"

bool cpuMapRead(Mapper000 mapper000, uint16_t addr, uint32_t *mapped_addr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        *mapped_addr = addr & (mapper000.PRGBanks> 1? 0x7FFF : 0x3FFF); // change the location of reading or rom file, depending on the number of banks supplied. Here 32K rom
        return true;
    }
    return false;
}
bool cpuMapWrite(Mapper000 mapper000, uint16_t addr, uint32_t *mapped_addr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        return true;
    }
    return false;
}
bool ppuMapRead(Mapper000 mapper000, uint16_t addr, uint32_t *mapped_addr) {
    if (addr >= 0x0000 && addr <= 0x1FFF) {
        *mapped_addr = addr;
        return true;
    }
    return false;
}
bool ppuMapWrite(Mapper000 mapper000, uint16_t addr, uint32_t *mapped_addr) {
        //if (addr >= 0x0000 && addr <= 0x1FFF) {
        //    return true;
        //}
        return false;
}

