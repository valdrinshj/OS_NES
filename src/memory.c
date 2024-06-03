#include "memory.h"

void MemoryInit(Memory *memory) {
    for (uint16_t addr = 0x0000; addr < 0xFFFF; ++addr) {
        memory->RAM[addr] = 0x00;
    }
}

uint8_t MemoryRead(Memory *memory, uint16_t addr) {
    return memory->RAM[addr & 0x07FF];  // RAM Mirroring. 8Kb addresable range is morrered into the first 2Kb of RAM
}

void MemoryWrite(Memory *memory, uint16_t addr, uint8_t data) {
    memory->RAM[addr & 0x07FF] = data;
}