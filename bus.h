#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct{

} Bus;


//Functions

uint8_t BusRead(Bus *bus, uint16_t addr);
void BusWrite(Bus *bus, uint16_t addr, uint8_t data);
// void NesReset(Bus *bus);
// void NesClock(Bus *bus);
#endif
