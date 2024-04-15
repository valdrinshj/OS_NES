#include "bus.h"


int i;

for (i = 0; i < sizeof(ram); i++) {
ram[i] = 0x00;
}

void BusWrite(uint16_t addr, uint8_t data) {

}
uint8_t BusRead(uint16_t addr, bool bReadOnly) {

}