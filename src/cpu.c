#include "cpu.h"

#include <stdlib.h>
#include <string.h>


#define HEX_CHARS "0123456789ABCDEF"

//Initialize Cpu
static Cpu cpu = {0};

typedef struct {
    char name[4];
    uint8_t (*operate)(void);
    uint8_t (*addrmode)(void);
    uint8_t cycles;
} CpuInstruction;

//List of cpu instructions {"mnemonic", implement function, address mode, clock cycles}
static CpuInstruction LOOKUP [16*16]= {
        { "BRK", BRK, IMP, 7 },{ "ORA", ORA, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 3 },{ "ORA", ORA, ZP0, 3 },{ "ASL", ASL, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PHP", PHP, IMP, 3 },{ "ORA", ORA, IMM, 2 },{ "ASL", ASL, IMP, 2 },{ "???", XXX, IMP, 2 },{ "???", NOP, IMP, 4 },{ "ORA", ORA, ABS, 4 },{ "ASL", ASL, ABS, 6 },{ "???", XXX, IMP, 6 },
        { "BPL", BPL, REL, 2 },{ "ORA", ORA, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "ORA", ORA, ZPX, 4 },{ "ASL", ASL, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "CLC", CLC, IMP, 2 },{ "ORA", ORA, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "ORA", ORA, ABX, 4 },{ "ASL", ASL, ABX, 7 },{ "???", XXX, IMP, 7 },
        { "JSR", JSR, ABS, 6 },{ "AND", AND, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "BIT", BIT, ZP0, 3 },{ "AND", AND, ZP0, 3 },{ "ROL", ROL, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PLP", PLP, IMP, 4 },{ "AND", AND, IMM, 2 },{ "ROL", ROL, IMP, 2 },{ "???", XXX, IMP, 2 },{ "BIT", BIT, ABS, 4 },{ "AND", AND, ABS, 4 },{ "ROL", ROL, ABS, 6 },{ "???", XXX, IMP, 6 },
        { "BMI", BMI, REL, 2 },{ "AND", AND, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "AND", AND, ZPX, 4 },{ "ROL", ROL, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "SEC", SEC, IMP, 2 },{ "AND", AND, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "AND", AND, ABX, 4 },{ "ROL", ROL, ABX, 7 },{ "???", XXX, IMP, 7 },
        { "RTI", RTI, IMP, 6 },{ "EOR", EOR, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 3 },{ "EOR", EOR, ZP0, 3 },{ "LSR", LSR, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PHA", PHA, IMP, 3 },{ "EOR", EOR, IMM, 2 },{ "LSR", LSR, IMP, 2 },{ "???", XXX, IMP, 2 },{ "JMP", JMP, ABS, 3 },{ "EOR", EOR, ABS, 4 },{ "LSR", LSR, ABS, 6 },{ "???", XXX, IMP, 6 },
        { "BVC", BVC, REL, 2 },{ "EOR", EOR, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "EOR", EOR, ZPX, 4 },{ "LSR", LSR, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "CLI", CLI, IMP, 2 },{ "EOR", EOR, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "EOR", EOR, ABX, 4 },{ "LSR", LSR, ABX, 7 },{ "???", XXX, IMP, 7 },
        { "RTS", RTS, IMP, 6 },{ "ADC", ADC, IZX, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 3 },{ "ADC", ADC, ZP0, 3 },{ "ROR", ROR, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "PLA", PLA, IMP, 4 },{ "ADC", ADC, IMM, 2 },{ "ROR", ROR, IMP, 2 },{ "???", XXX, IMP, 2 },{ "JMP", JMP, IND, 5 },{ "ADC", ADC, ABS, 4 },{ "ROR", ROR, ABS, 6 },{ "???", XXX, IMP, 6 },
        { "BVS", BVS, REL, 2 },{ "ADC", ADC, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "ADC", ADC, ZPX, 4 },{ "ROR", ROR, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "SEI", SEI, IMP, 2 },{ "ADC", ADC, ABY, 4 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "ADC", ADC, ABX, 4 },{ "ROR", ROR, ABX, 7 },{ "???", XXX, IMP, 7 },
        { "???", NOP, IMP, 2 },{ "STA", STA, IZX, 6 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 6 },{ "STY", STY, ZP0, 3 },{ "STA", STA, ZP0, 3 },{ "STX", STX, ZP0, 3 },{ "???", XXX, IMP, 3 },{ "DEY", DEY, IMP, 2 },{ "???", NOP, IMP, 2 },{ "TXA", TXA, IMP, 2 },{ "???", XXX, IMP, 2 },{ "STY", STY, ABS, 4 },{ "STA", STA, ABS, 4 },{ "STX", STX, ABS, 4 },{ "???", XXX, IMP, 4 },
        { "BCC", BCC, REL, 2 },{ "STA", STA, IZY, 6 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 6 },{ "STY", STY, ZPX, 4 },{ "STA", STA, ZPX, 4 },{ "STX", STX, ZPY, 4 },{ "???", XXX, IMP, 4 },{ "TYA", TYA, IMP, 2 },{ "STA", STA, ABY, 5 },{ "TXS", TXS, IMP, 2 },{ "???", XXX, IMP, 5 },{ "???", NOP, IMP, 5 },{ "STA", STA, ABX, 5 },{ "???", XXX, IMP, 5 },{ "???", XXX, IMP, 5 },
        { "LDY", LDY, IMM, 2 },{ "LDA", LDA, IZX, 6 },{ "LDX", LDX, IMM, 2 },{ "???", XXX, IMP, 6 },{ "LDY", LDY, ZP0, 3 },{ "LDA", LDA, ZP0, 3 },{ "LDX", LDX, ZP0, 3 },{ "???", XXX, IMP, 3 },{ "TAY", TAY, IMP, 2 },{ "LDA", LDA, IMM, 2 },{ "TAX", TAX, IMP, 2 },{ "???", XXX, IMP, 2 },{ "LDY", LDY, ABS, 4 },{ "LDA", LDA, ABS, 4 },{ "LDX", LDX, ABS, 4 },{ "???", XXX, IMP, 4 },
        { "BCS", BCS, REL, 2 },{ "LDA", LDA, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 5 },{ "LDY", LDY, ZPX, 4 },{ "LDA", LDA, ZPX, 4 },{ "LDX", LDX, ZPY, 4 },{ "???", XXX, IMP, 4 },{ "CLV", CLV, IMP, 2 },{ "LDA", LDA, ABY, 4 },{ "TSX", TSX, IMP, 2 },{ "???", XXX, IMP, 4 },{ "LDY", LDY, ABX, 4 },{ "LDA", LDA, ABX, 4 },{ "LDX", LDX, ABY, 4 },{ "???", XXX, IMP, 4 },
        { "CPY", CPY, IMM, 2 },{ "CMP", CMP, IZX, 6 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 8 },{ "CPY", CPY, ZP0, 3 },{ "CMP", CMP, ZP0, 3 },{ "DEC", DEC, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "INY", INY, IMP, 2 },{ "CMP", CMP, IMM, 2 },{ "DEX", DEX, IMP, 2 },{ "???", XXX, IMP, 2 },{ "CPY", CPY, ABS, 4 },{ "CMP", CMP, ABS, 4 },{ "DEC", DEC, ABS, 6 },{ "???", XXX, IMP, 6 },
        { "BNE", BNE, REL, 2 },{ "CMP", CMP, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "CMP", CMP, ZPX, 4 },{ "DEC", DEC, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "CLD", CLD, IMP, 2 },{ "CMP", CMP, ABY, 4 },{ "NOP", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "CMP", CMP, ABX, 4 },{ "DEC", DEC, ABX, 7 },{ "???", XXX, IMP, 7 },
        { "CPX", CPX, IMM, 2 },{ "SBC", SBC, IZX, 6 },{ "???", NOP, IMP, 2 },{ "???", XXX, IMP, 8 },{ "CPX", CPX, ZP0, 3 },{ "SBC", SBC, ZP0, 3 },{ "INC", INC, ZP0, 5 },{ "???", XXX, IMP, 5 },{ "INX", INX, IMP, 2 },{ "SBC", SBC, IMM, 2 },{ "NOP", NOP, IMP, 2 },{ "???", SBC, IMP, 2 },{ "CPX", CPX, ABS, 4 },{ "SBC", SBC, ABS, 4 },{ "INC", INC, ABS, 6 },{ "???", XXX, IMP, 6 },
        { "BEQ", BEQ, REL, 2 },{ "SBC", SBC, IZY, 5 },{ "???", XXX, IMP, 2 },{ "???", XXX, IMP, 8 },{ "???", NOP, IMP, 4 },{ "SBC", SBC, ZPX, 4 },{ "INC", INC, ZPX, 6 },{ "???", XXX, IMP, 6 },{ "SED", SED, IMP, 2 },{ "SBC", SBC, ABY, 4 },{ "NOP", NOP, IMP, 2 },{ "???", XXX, IMP, 7 },{ "???", NOP, IMP, 4 },{ "SBC", SBC, ABX, 4 },{ "INC", INC, ABX, 7 },{ "???", XXX, IMP, 7 },
};

Cpu *cpu_get() {
    return &cpu;
}
void cpu_init() {
    cpu.bus = NULL;
    cpu.PC = 0x0000;
    cpu.SP = 0x00;
    cpu.A = 0x00;
    cpu.X = 0x00;
    cpu.Y = 0x00;
    cpu.status = 0;   // CPU Status set all flags to 0
    cpu.cycles = 0;
    cpu.fetched = 0x00;
    cpu.addr_abs = 0x0000;
    cpu.addr_rel = 0x0000;
    cpu.opcode = 0x00;
}
void cpu_connect_bus(Bus *bus) {
    cpu.bus = bus;
}

uint8_t cpu_read(uint16_t addr) {
    return busRead(cpu.bus, addr);
}

void cpu_write(uint16_t addr, uint8_t data) {
    busWrite(cpu.bus, addr, data);
}

void cpu_set_flag(CpuStatusFlag flag, bool one) {
    if (one) {
        cpu.status |= flag;
    }
    else {
        cpu.status &= ~flag;
    }
}

uint8_t cpu_get_flag(CpuStatusFlag flag) {
    return ((cpu.status & flag) > 0) ? 1 : 0;
}
static char* hex(uint32_t n, uint8_t d, char *dst) {
    memset(dst, 0, d);
    int i;
    dst[d] = 0;
    for (i = d - 1; i >= 0; i--, n >>= 4) {
        dst[i] = HEX_CHARS[n & 0xF];
    }
    return dst;
}

bool CpuComplete() {
    return cpu.cycles == 0;
}
void cpu_disassemble(uint16_t nStart, uint16_t nStop, char* mapLines [0xFFFF]) {

    uint32_t addr = nStart;
	uint8_t value = 0x00, lo = 0x00, hi = 0x00;
	uint16_t line_addr;
    char *sInst = (char*)calloc(1024, 1);
    char hex_aux[16];

	while (addr <= (uint32_t)nStop) {
        memset(sInst, 0, 1024);
		line_addr = addr;

		// Prefix line with instruction address

        strcat(sInst, "$");
        strcat(sInst, hex(addr, 4, hex_aux));
        strcat(sInst, ": ");

		// Read instruction, and get its readable name
		uint8_t opcode = cpu_read(addr);
        addr++;
		strcat(sInst, LOOKUP[opcode].name);
        strcat(sInst, " ");

		// Get oprands from desired locations, and form the
		// instruction based upon its addressing mode. These
		// routines mimmick the actual fetch routine of the
		// 6502 in order to get accurate data as part of the
		// instruction

		if (LOOKUP[opcode].addrmode == IMP)
		{
            strcat(sInst, " {IMP}");
		}
		else if (LOOKUP[opcode].addrmode == IMM)
		{
			value = cpu_read(addr); addr++;
            strcat(sInst, "#$");
            strcat(sInst, hex(value, 2, hex_aux));
            strcat(sInst, " {IMM}");
		}
		else if (LOOKUP[opcode].addrmode == ZP0)
		{
			lo = cpu_read(addr); addr++;
			hi = 0x00;
            strcat(sInst, "$");
            strcat(sInst, hex(lo, 2, hex_aux));
            strcat(sInst, " {ZP0}");
		}
		else if (LOOKUP[opcode].addrmode == ZPX)
		{
			lo = cpu_read(addr); addr++;
			hi = 0x00;
            strcat(sInst, "$");
            strcat(sInst, hex(lo, 2, hex_aux));
            strcat(sInst, ", X {ZPX}");
		}
		else if (LOOKUP[opcode].addrmode == ZPY)
		{
			lo = cpu_read(addr); addr++;
			hi = 0x00;
            strcat(sInst, "$");
            strcat(sInst, hex(lo, 2, hex_aux));
            strcat(sInst, ", Y {ZPY}");
		}
		else if (LOOKUP[opcode].addrmode == IZX)
		{
			lo = cpu_read(addr); addr++;
			hi = 0x00;
            strcat(sInst, "($");
            strcat(sInst, hex(lo, 2, hex_aux));
            strcat(sInst, ", X) {IZX}");
		}
		else if (LOOKUP[opcode].addrmode == IZY)
		{
			lo = cpu_read(addr); addr++;
			hi = 0x00;
            strcat(sInst, "($");
            strcat(sInst, hex(lo, 2, hex_aux));
            strcat(sInst, "), Y {IZY}");
		}
		else if (LOOKUP[opcode].addrmode == ABS)
		{
			lo = cpu_read(addr); addr++;
			hi = cpu_read(addr); addr++;
            strcat(sInst, "$");
            strcat(sInst, hex((uint16_t)(hi << 8) | lo, 4, hex_aux));
            strcat(sInst, " {ABS}");
		}
		else if (LOOKUP[opcode].addrmode == ABX)
		{
			lo = cpu_read(addr); addr++;
			hi = cpu_read(addr); addr++;
            strcat(sInst, "$");
            strcat(sInst, hex((uint16_t)(hi << 8) | lo, 4, hex_aux));
            strcat(sInst, ", X {ABX}");
		}
		else if (LOOKUP[opcode].addrmode == ABY)
		{
			lo = cpu_read(addr); addr++;
			hi = cpu_read(addr); addr++;
            strcat(sInst, "$");
            strcat(sInst, hex((uint16_t)(hi << 8) | lo, 4, hex_aux));
            strcat(sInst, ", Y {ABY}");
		}
		else if (LOOKUP[opcode].addrmode == IND)
		{
			lo = cpu_read(addr); addr++;
			hi = cpu_read(addr); addr++;
            strcat(sInst, "($");
            strcat(sInst, hex((uint16_t)(hi << 8) | lo, 4, hex_aux));
            strcat(sInst, ") {IND}");
		}
		else if (LOOKUP[opcode].addrmode == REL)
		{
			value = cpu_read(addr); addr++;
            strcat(sInst, "$");
            strcat(sInst, hex(value, 2, hex_aux));
            strcat(sInst, " [$");
            strcat(sInst, hex(addr + (int8_t)value, 4, hex_aux));
            strcat(sInst, "] {REL}");
		}

		// Add the formed string to a std::map, using the instruction's
		// address as the key. This makes it convenient to look for later
		// as the instructions are variable in length, so a straight up
		// incremental index is not sufficient.
		mapLines[line_addr] = strdup(sInst);
    }

}
void cpu_clock() {
    if (cpu.cycles ==  0) {
        cpu.opcode = cpu_read(cpu.PC);
        cpu_set_flag(U, true);
        cpu.PC++;

        //Get starting number of cycles
        CpuInstruction instruction = LOOKUP[cpu.opcode];
        cpu.cycles = instruction.cycles;
        uint8_t  addrmode_additional_cycle = instruction.addrmode();
        uint8_t operate_additional_cycle = instruction.operate();
        cpu.cycles += addrmode_additional_cycle & operate_additional_cycle;
        cpu_set_flag(U, true);
    }
    cpu.cycles--;
}

//ADDRESSING MODES

//Implied: No data, doesn't need to do anything. Or operating upon the accumulator.
uint8_t IMP() {
    cpu.fetched = cpu.A;
    return 0;
}

//Immediate Mode Addressing: Data is supplied, going to be the next byte.
uint8_t IMM() {
    cpu.addr_abs = cpu.PC++;
    return 0;
}

//Zero Page Addressing
uint8_t ZP0() {
    cpu.addr_abs = cpu_read(cpu.PC);
    cpu.PC++;
    cpu.addr_abs &= 0x00FF;
    return 0;
}

//Zero page addressing (X offset)
uint8_t ZPX() {
    cpu.addr_abs = cpu_read(cpu.PC) + cpu.X;
    cpu.addr_abs &= 0x00FF;
    return 0;
}

//Zero page addressing (Y offset)
uint8_t ZPY() {
    cpu.addr_abs = cpu_read(cpu.PC) + cpu.Y;
    cpu.addr_abs &= 0x00FF;
    return 0;
}

//Absolute Address
uint8_t ABS() {
    uint16_t lo = cpu_read(cpu.PC);
    cpu.PC++;
    uint16_t hi = cpu_read(cpu.PC);
    cpu.PC++;

    cpu.addr_abs = (hi << 8) | lo;

    return 0;
}

//Absolute Address (X)
uint8_t ABX() {
    uint16_t lo = cpu_read(cpu.PC);
    cpu.PC++;
    uint16_t hi = cpu_read(cpu.PC);
    cpu.PC++;

    cpu.addr_abs = ((hi << 8) | lo) + cpu.X;

    // If the addr is in a new page, then we may need another clock cycle
    if ((cpu.addr_abs & 0xFF00) != (hi << 8)) {
        return 1;
    }

    return 0;
}

//Absolute Address (Y)
uint8_t ABY() {
    uint16_t lo = cpu_read(cpu.PC);
    cpu.PC++;
    uint16_t hi = cpu_read(cpu.PC);
    cpu.PC++;

    cpu.addr_abs = ((hi << 8) | lo) + cpu.Y;

    // If the addr is in a new page, then we may need another clock cycle
    if ((cpu.addr_abs & 0xFF00) != (hi << 8)) {
        return 1;
    }

    return 0;
}

//Indirect Addressing
uint8_t IND() {
    uint16_t ptr_lo = cpu_read(cpu.PC);
    cpu.PC++;
    uint16_t ptr_hi = cpu_read(cpu.PC);
    cpu.PC++;

    //NES HARDWARE BUG HANDLING: Normally it would switch the page.
    uint16_t ptr = (ptr_hi << 8) | ptr_lo;
    if (ptr_lo == 0x00FF) {
        cpu.addr_abs = (cpu_read(ptr & 0xFF00) << 8) | cpu_read(ptr + 0);
    }
        // Normal behaviour
    else {
        cpu.addr_abs = (cpu_read(ptr + 1) << 8) | cpu_read(ptr + 0);
    }
    return 0;
}

//Indirect Addressing of the Zero page with X offset
uint8_t IZX() {
    uint16_t t = cpu_read(cpu.PC);
    cpu.PC++;
    uint16_t lo = cpu_read((uint16_t)(t + (uint16_t)cpu.X) & 0x00FF);
    uint16_t hi = cpu_read((uint16_t)(t + (uint16_t)cpu.X + 1) & 0x00FF);
    cpu.addr_abs = (hi << 8) | lo;

    return 0;
}

//Indirect Addressing of the Zero page with Y offset
uint8_t IZY() {
    uint16_t t = cpu_read(cpu.PC);
    cpu.PC++;
    uint16_t lo = cpu_read(t & 0x00FF);
    uint16_t hi = cpu_read((t + 1) & 0x00FF);
    cpu.addr_abs = ((hi << 8) | lo) + cpu.Y;
    // If the addr is in a new page, then we may need another clock cycle
    if ((cpu.addr_abs & 0xFF00) != (hi << 8)) {
        return 1;
    }
    return 0;
}

//Relative addressing mode
uint8_t REL() {
    cpu.addr_rel = cpu_read(cpu.PC);
    cpu.PC++;
    if (cpu.addr_rel & 0x80) {      // 0x80 = 128. Checking 7th bit.
        cpu.addr_rel |= 0xFF00;
    }
    return 0;
}

//TODO: implement instructions
//INSTRUCTIONS

uint8_t cpu_fetch() {
    if (LOOKUP[cpu.opcode].addrmode != IMP) {
        cpu.fetched = cpu_read(cpu.addr_abs);
    }
    return cpu.fetched;
}

uint8_t AND() {
    cpu_fetch();
    cpu.A = cpu.A & cpu.fetched;
    cpu_set_flag(Z, cpu.A == 0x00);
    cpu_set_flag(N, cpu.A & 0x80);
    return 1;
}

uint8_t BCS() {
    if(cpu_get_flag(C) == 1) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BCC() {
    if(cpu_get_flag(C) == 0) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BEQ() {
    if(cpu_get_flag(Z) == 1) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BMI() {
    if(cpu_get_flag(N) == 1) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BNE() {
    if(cpu_get_flag(Z) == 0) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BPL() {
    if(cpu_get_flag(N) == 0) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BVC() {
    if(cpu_get_flag(V) == 0) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BVS() {
    if(cpu_get_flag(V) == 1) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t CLC() {
    cpu_set_flag(C, false);
    return 0;
}

uint8_t CLD() {
    cpu_set_flag(D, false);
    return 0;
}

uint8_t CLI() {
    cpu_set_flag(I, false);
    return 0;
}

uint8_t CLV() {
    cpu_set_flag(V, false);
    return 0;
}

// A += M + C
uint8_t ADC() {
    cpu_fetch();
    cpu.temp = (uint16_t)cpu.A + (uint16_t)cpu.fetched + (uint16_t) cpu_get_flag(C);
    cpu_set_flag(C, cpu.temp > 255);
    cpu_set_flag(Z, (cpu.temp & 0x0FF) == 0);
    cpu_set_flag(V, (~((uint16_t)cpu.A ^ (uint16_t)cpu.fetched) & ((uint16_t)cpu.A ^ (uint16_t) cpu.temp)) & 0x0080);
    cpu_set_flag(N, cpu.temp & 0x80);

    cpu.A = cpu.temp & 0x00FF;
    return 1;
}

uint8_t SBC() {
    cpu_fetch();
    uint16_t inverted = ((uint16_t )(cpu.fetched)) ^ 0x00FF;
    cpu.temp = (uint16_t )cpu.A + inverted + (uint16_t )cpu_get_flag(C);
    cpu_set_flag(C, cpu.temp & 0xFF00);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0);
    cpu_set_flag(V, (cpu.temp ^ (uint16_t)cpu.A) & (cpu.temp ^ inverted) & 0x0080);
    cpu_set_flag(N, cpu.temp & 0x0080);
    cpu.A = cpu.temp & 0x00FF;
    return 1;
}

uint8_t PHA() {
    cpu_write(0x0100 + cpu.SP, cpu.A);
    cpu.SP--;
    return 0;
}

uint8_t PLA() {
    cpu.SP++;
    cpu.A = cpu_read(0x0100 + cpu.SP);
    cpu_set_flag(Z, cpu.A == 0x00);
    cpu_set_flag(N, cpu.A & 0x80);
    return 0;
}

void cpu_reset() {

    cpu.A = 0x00;
    cpu.X = 0x00;
    cpu.Y = 0x00;
    cpu.SP = 0xFD;
    cpu.status = 0x00 | U;

    cpu.addr_abs = 0xFFFC;
    uint16_t lo = cpu_read(cpu.addr_abs + 0);
    uint16_t hi = cpu_read(cpu.addr_abs + 1);

    cpu.PC = (hi << 8) | lo;

    cpu.addr_rel = 0x0000;
    cpu.addr_abs = 0x0000;
    cpu.fetched = 0x00;

    cpu.cycles = 8;
}

void cpu_interrupt_request() {
    if(cpu_get_flag(I) == 0) {
        cpu_write(0x0100 + cpu.SP, (cpu.PC >> 8) & 0x00FF);
        cpu.SP--;
        cpu_write(0x0100 + cpu.SP, cpu.PC & 0x00FF);
        cpu.SP--;

        cpu_set_flag(B,0);
        cpu_set_flag(U,1);
        cpu_set_flag(I,1);
        cpu_write(0x0100 + cpu.SP, cpu.status);
        cpu.SP--;

        cpu.addr_abs = 0xFFFE;
        uint16_t lo = cpu_read(cpu.addr_abs + 0);
        uint16_t hi = cpu_read(cpu.addr_abs + 1);
        cpu.PC = (hi << 8) | lo;

        cpu.cycles = 7;
    }
}

void cpu_non_mask_interrupt() {
    cpu_write(0x0100 + cpu.SP, (cpu.PC >> 8) & 0x00FF);
    cpu.SP--;
    cpu_write(0x0100 + cpu.SP, cpu.PC & 0x00FF);
    cpu.SP--;

    cpu_set_flag(B,0);
    cpu_set_flag(U,1);
    cpu_set_flag(I,1);
    cpu_write(0x0100 + cpu.SP, cpu.status);
    cpu.SP--;

    cpu.addr_abs = 0xFFFE;
    uint16_t lo = cpu_read(cpu.addr_abs + 0);
    uint16_t hi = cpu_read(cpu.addr_abs + 1);
    cpu.PC = (hi << 8) | lo;

    cpu.cycles = 8;
}

uint8_t RTI() {
    cpu.SP++;
    cpu.status = cpu_read(0x0100 + cpu.SP);
    cpu.status &= ~B;
    cpu.status &= ~U;

    cpu.SP++;
    cpu.PC = (uint16_t)cpu_read(0x0100 + cpu.SP);
    cpu.SP++;
    cpu.PC |= (uint16_t)cpu_read(0x0100 + cpu.SP) << 8;

    return 0;
}

// TODO: Instructions that weren't covered in the video:

uint8_t ASL() {
    cpu_fetch();
    cpu.temp = (uint16_t)cpu.fetched << 1;
    cpu_set_flag(C, (cpu.temp & 0xFF00) > 0);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x00);
    cpu_set_flag(N, cpu.temp & 0x80);
    if (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        cpu_write(cpu.addr_abs, cpu.temp & 0x00FF);
    return 0;
}

uint8_t BIT() {
    cpu_fetch();
    cpu.temp = cpu.A & cpu.fetched;
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x00);
    cpu_set_flag(N, cpu.fetched & (1 << 7));
    cpu_set_flag(V, cpu.fetched & (1 << 6));
    return 0;
}

uint8_t BRK() {
    cpu.PC++;

    cpu_set_flag(I, 1);
    cpu_write(0x0100 + cpu.SP, (cpu.PC >> 8) & 0x00FF);
    cpu.SP--;
    cpu_write(0x0100 + cpu.SP, cpu.PC & 0x00FF);
    cpu.SP--;

    cpu_set_flag(B, 1);
    cpu_write(0x0100 + cpu.SP, cpu.status);
    cpu.SP--;
    cpu_set_flag(B, 0);

    cpu.PC = (uint16_t) cpu_read(0xFFFE) | ((uint16_t) cpu_read(0xFFFF) << 8);
    return 0;
}

uint8_t CMP() {
    cpu_fetch();
    cpu.temp = (uint16_t)cpu.A - (uint16_t)cpu.fetched;
    cpu_set_flag(C, cpu.A >= cpu.fetched);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x0000);
    cpu_set_flag(N, cpu.temp & 0x0080);
    return 1;
}

uint8_t CPX() {
    cpu_fetch();
    cpu.temp = (uint16_t)cpu.X - (uint16_t)cpu.fetched;
    cpu_set_flag(C, cpu.X >= cpu.fetched);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x0000);
    cpu_set_flag(N, cpu.temp & 0x0080);
    return 0;
}

uint8_t CPY() {
    cpu_fetch();
    cpu.temp = (uint16_t)cpu.Y - (uint16_t)cpu.fetched;
    cpu_set_flag(C, cpu.Y >= cpu.fetched);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x0000);
    cpu_set_flag(N, cpu.temp & 0x0080);
    return 0;
}

uint8_t DEC() {
    cpu_fetch();
    cpu.temp = cpu.fetched - 1;
    cpu_write(cpu.addr_abs, cpu.temp & 0x00FF);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x0000);
    cpu_set_flag(N, cpu.temp & 0x0080);
    return 0;
}

uint8_t DEX() {
    cpu.X--;
    cpu_set_flag(Z, cpu.X == 0x00);
    cpu_set_flag(N, cpu.X & 0x80);
    return 0;
}

uint8_t DEY() {
    cpu.Y--;
    cpu_set_flag(Z, cpu.Y == 0x00);
    cpu_set_flag(N, cpu.Y & 0x80);
    return 0;
}

uint8_t EOR() {
    cpu_fetch();
    cpu.A = cpu.A ^ cpu.fetched;
    cpu_set_flag(Z, cpu.A == 0x00);
    cpu_set_flag(N, cpu.A & 0x80);
    return 1;
}

uint8_t INC() {
    cpu_fetch();
    cpu.temp = cpu.fetched + 1;
    cpu_write(cpu.addr_abs, cpu.temp & 0x00FF);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x0000);
    cpu_set_flag(N, cpu.temp & 0x0080);
    return 0;
}

uint8_t INX() {
    cpu.X++;
    cpu_set_flag(Z, cpu.X == 0x00);
    cpu_set_flag(N, cpu.X & 0x80);
    return 0;
}

uint8_t INY() {
    cpu.Y++;
    cpu_set_flag(Z, cpu.Y == 0x00);
    cpu_set_flag(N, cpu.Y & 0x80);
    return 0;
}

uint8_t JMP() {
    cpu.PC = cpu.addr_abs;
    return 0;
}

uint8_t JSR() {
    cpu.PC--;
    cpu_write(0x0100 + cpu.SP, (cpu.PC >> 8) & 0x00FF);
    cpu.SP--;
    cpu_write(0x0100 + cpu.SP, cpu.PC & 0x00FF);
    cpu.SP--;

    cpu.PC = cpu.addr_abs;
    return 0;
}

uint8_t LDA() {
    cpu_fetch();
    cpu.A = cpu.fetched;
    cpu_set_flag(Z, cpu.A == 0x00);
    cpu_set_flag(N, cpu.A & 0x80);
    return 1;
}

uint8_t LDX() {
    cpu_fetch();
    cpu.X = cpu.fetched;
    cpu_set_flag(Z, cpu.X == 0x00);
    cpu_set_flag(N, cpu.X & 0x80);
    return 1;
}

uint8_t LDY() {
    cpu_fetch();
    cpu.Y = cpu.fetched;
    cpu_set_flag(Z, cpu.Y == 0x00);
    cpu_set_flag(N, cpu.Y & 0x80);
    return 1;
}

uint8_t LSR() {
    cpu_fetch();
    cpu_set_flag(C, cpu.fetched & 0x0001);
    cpu.temp = cpu.fetched >> 1;
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x0000);
    cpu_set_flag(N, cpu.temp & 0x0080);

    if  (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        cpu_write(cpu.addr_abs, cpu.temp & 0x00FF);
    return 0;
}

uint8_t NOP() {
    switch (cpu.opcode) {
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            return 1;
        default:
            break;
    }
    return 0;
}

uint8_t ORA() {
    cpu_fetch();
    cpu.A = cpu.A | cpu.fetched;
    cpu_set_flag(Z, cpu.A == 0x00);
    cpu_set_flag(N, cpu.A & 0x80);
    return 1;
}

uint8_t PHP() {
    cpu_write(0x0100 + cpu.SP, cpu.status | B | U );
    cpu_set_flag(B, 0);
    cpu_set_flag(U, 0);
    cpu.SP--;
    return 0;
}

uint8_t PLP() {
    cpu.SP++;
    cpu.status = cpu_read(0x0100 + cpu.SP);
    cpu_set_flag(U, 1);
    return 0;
}

uint8_t ROL() {
    cpu_fetch();
    cpu.temp = (uint16_t)(cpu.fetched << 1) | cpu_get_flag(C);
    cpu_set_flag(C, cpu.temp & 0xFF00);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x0000);
    cpu_set_flag(N, cpu.temp & 0x0080);
    if (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        cpu_write(cpu.addr_abs, cpu.temp & 0x00FF);
    return 0;
}

uint8_t ROR() {
    cpu_fetch();
    cpu.temp = (uint16_t)(cpu_get_flag(C) << 7) | (cpu.fetched >> 1);
    cpu_set_flag(C, cpu.fetched & 0x01);
    cpu_set_flag(Z, (cpu.temp & 0x00FF) == 0x00);
    cpu_set_flag(N, cpu.temp & 0x0080);
    if (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        cpu_write(cpu.addr_abs, cpu.temp & 0x00FF);
    return 0;
}

uint8_t RTS() {
    cpu.SP++;
    cpu.PC = (uint16_t) cpu_read(0x0100 + cpu.SP);
    cpu.SP++;
    cpu.SP |= (uint16_t) cpu_read(0x0100 + cpu.SP) << 8;
    cpu.PC++;
    return 0;
}

uint8_t SEC() {
    cpu_set_flag(C, true);
    return 0;
}

uint8_t SED() {
    cpu_set_flag(D, true);
    return 0;
}

uint8_t SEI() {
    cpu_set_flag(I, true);
    return 0;
}

uint8_t STA() {
    cpu_write(cpu.addr_abs, cpu.A);
    return 0;
}

uint8_t STX() {
    cpu_write(cpu.addr_abs, cpu.X);
    return 0;
}

uint8_t STY() {
    cpu_write(cpu.addr_abs, cpu.Y);
    return 0;
}

uint8_t TAX() {
    cpu.X = cpu.A;
    cpu_set_flag(Z, cpu.X == 0x00);
    cpu_set_flag(N, cpu.X & 0x80);
    return 0;
}

uint8_t TAY() {
    cpu.Y = cpu.A;
    cpu_set_flag(Z, cpu.Y == 0x00);
    cpu_set_flag(N, cpu.Y & 0x80);
    return 0;
}

uint8_t TSX() {
    cpu.X = cpu.SP;
    cpu_set_flag(Z, cpu.X == 0x00);
    cpu_set_flag(N, cpu.X & 0x80);
    return 0;
}

uint8_t TXA() {
    cpu.A = cpu.X;
    cpu_set_flag(Z, cpu.A == 0x00);
    cpu_set_flag(N, cpu.A & 0x80);
    return 0;
}

uint8_t TXS() {
    cpu.SP = cpu.X;
    return 0;
}

uint8_t TYA() {
    cpu.A = cpu.Y;
    cpu_set_flag(Z, cpu.A == 0x00);
    cpu_set_flag(N, cpu.A & 0x80);
    return 0;
}

uint8_t XXX() {
    return 0;
}


