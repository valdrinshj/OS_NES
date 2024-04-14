#ifndef PROJECT_BUS_H
#define PROJECT_BUS_H

#include <stdint.h>
#include <stdbool.h>
#include "cpu.h"

typedef struct{

}Bus;

void write(uint16_t addr, uint8_t data);
uint8_t read(uint16_t addr);


#endif //PROJECT_BUS_H
