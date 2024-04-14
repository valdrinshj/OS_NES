#ifndef IMP_6502_CPU
#define IMP_6502_CPU

#include <stdint.h>
#include "bus.h"

typedef struct {
    Bus *bus;
    uint16_t PC; //program counter: next program bite stored
    uint8_t SP; //stack pointer: points to empty stack mostly
    uint8_t A; // accumulator: holds the results of arithmetic and logical operations
    uint8_t X; // index register x: Often used for indexing arrays and accessing data structures in memory.
    uint8_t Y; // index register y: Typically used as a loop counter or for pointer manipulation.
    uint8_t status; // status register

} Cpu;

typedef enum {
    C = (1 << 0),   // bit 0: Carry
    Z = (1 << 1),   // bit 1: Zero
    I = (1 << 2),   // bit 2: Interrupt Disable
    D = (1 << 3),   // bit 3: Decimal (will not be used)
    B = (1 << 4),   // bit 4: Break
    U = (1 << 5),   // bit 5: Unused
    V = (1 << 6),   // bit 6: Overflow
    N = (1 << 7),   // bit 7: Negative
} CpuStatusFlag;
extern struct cpu cpu;



void cpu_reset(void);
uint8_t cpu_extract_sr(uint8_t flag);
uint8_t cpu_mod_sr(uint8_t flag, uint8_t val);
uint8_t cpu_fetch(uint16_t addr);
uint8_t cpu_write(uint16_t addr, uint8_t data);
void cpu_exec();
void cpu_init(void);

#endif //IMP_6502_CPU
