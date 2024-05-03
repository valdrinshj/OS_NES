#ifndef INC_6502_MEM_H
#define INC_6502_MEM_H

typedef struct {

}Ppu;

//Communication with main bus
uint8_t cpuReadFromPpu(uint16_t addr, bool bReadOnly);
void cpuWriteToPpu(uint16_t addr, uint8_t data);

//Communication with ppu bus
uint8_t ppuRead(uint16_t addr, bool rReadOnly);
void ppuWrite(uint16_t addr, uint8_t data);

#endif
