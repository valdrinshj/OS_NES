#ifndef IMP_6502_CPU
#define IMP_6502_CPU

#include <stdint.h>

typedef struct {
    uint16_t pc; //program counter: next program bite stored
    uint8_t sp; //stack pointer: points to empty stack mostly
    uint8_t ac; // accumulator: holds the results of arithmetic and logical operations
    uint8_t x; // index register x: Often used for indexing arrays and accessing data structures in memory.
    uint8_t y; // index register y: Typically used as a loop counter or for pointer manipulation.
    uint8_t sr; // status register

} CPU;

#define C 0 // bit 0: Carry
#define Z 1 // bit 1: Zero
#define I 2 // bit 2: Interrupt
#define D 3 // bit 3: Decimal
#define B 4 // bit 4: Break
#define V 6 // bit 6: Overflow (V) --> no 5 bit register
#define N 7 // bit 7: Negative

extern struct cpu cpu;



void cpu_reset(void);
uint8_t cpu_extract_sr(uint8_t flag);
uint8_t cpu_mod_sr(uint8_t flag, uint8_t val);
uint8_t cpu_fetch(uint16_t addr);
uint8_t cpu_write(uint16_t addr, uint8_t data);
void cpu_exec();
void cpu_init(void);

#endif //IMP_6502_CPU
