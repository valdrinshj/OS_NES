#ifndef MEMORY_H
#define MEMORY_H

#include "stdint.h"

#define RAM_CAPAPCITY 2048

typedef struct {
     uint8_t RAM[RAM_CAPAPCITY];
} Memory;

void MemoryInit(Memory *memory);

uint8_t MemoryRead(Memory *memory, uint16_t addr);
void MemoryWrite(Memory *memory, uint16_t addr, uint8_t data);

#endif  // MEMORY_H