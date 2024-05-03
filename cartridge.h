#ifndef PROJECT_CARTRIDGE_H
#define PROJECT_CARTRIDGE_H

typedef struct {

}Cartridge;

//Communication with main bus
bool cpuRead(uint16_t addr, uint8_t  &data);
bool cpuWrite(uint16_t addr, uint8_t data);
//Communication with ppu bus
bool ppuRead(uint16_t addr, uint8_t  &data);
bool ppuWrite(uint16_t addr, uint8_t data);



#endif //PROJECT_CARTRIDGE_H
