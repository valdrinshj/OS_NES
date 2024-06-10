#include "cpu.h"
#include <stdlib.h>
#include <string.h>

#define ABS_SP(SP) ((0x0100) + (SP))
#define HEX_CHARS "0123456789ABCDEF"

static Cpu6502 cpu = {0};

void CpuInit() {
    cpu.bus = NULL;
    cpu.A = 0x00;
    cpu.X = 0x00;
    cpu.Y = 0x00;
    cpu.PC = 0x0000;
    cpu.SP = 0x00;
    cpu.status = 0;   // CPU Status set all flags to 0

    cpu.fetched = 0x00;
    cpu.addr_abs = 0x0000;
    cpu.addr_abs = 0x0000;
    cpu.opcode = 0x00;
    cpu.cycles = 0;
}

typedef struct {
    char name[4];
    uint8_t (*operate)(void);
    uint8_t (*addrmode)(void);
    uint8_t cycles;
} CpuInstruction;

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

//Bus connectivity
uint8_t Read(uint16_t addr) {
    return CpuRead(cpu.bus, addr);
}

void Write(uint16_t addr, uint8_t data) {
    CpuWrite(cpu.bus, addr, data);
}

//External Inputs
void Reset() {
    cpu.addr_abs = 0xFFFC;
    uint16_t lo = Read(cpu.addr_abs + 0);
    uint16_t hi = Read(cpu.addr_abs + 1);

    cpu.PC = (hi << 8) | lo;

    cpu.A = 0x00;
    cpu.X = 0x00;
    cpu.Y = 0x00;
    cpu.SP = 0xFD;
    cpu.status = 0x00 | U;

    cpu.addr_rel = 0x0000;
    cpu.addr_abs = 0x0000;
    cpu.fetched = 0x00;

    cpu.cycles = 8;
}

void ConnectBus(Bus *bus) {
    cpu.bus = bus;
}

uint8_t CpuGetFlag(CpuStatusFlag flag) {
    return ((cpu.status & flag) > 0) ? 1 : 0;
}

void CpuSetFlag(CpuStatusFlag flag, bool one) {
    if (one) {
        cpu.status |= flag;
    }
    else {
        cpu.status &= ~flag;
    }
}

void Irq() {
    if (CpuGetFlag(I) == 0){
        Write(ABS_SP(cpu.SP--), (cpu.PC >> 8) & 0x00FF);
        Write(ABS_SP(cpu.SP--), cpu.PC & 0x00FF);

        CpuSetFlag(B, 0);
        CpuSetFlag(U, 1);
        CpuSetFlag(I, 1);
        Write(ABS_SP(cpu.SP--), cpu.status);

        cpu.addr_abs = 0xFFFE;
        uint16_t lo = Read(cpu.addr_abs + 0);
        uint16_t hi = Read(cpu.addr_abs + 1);
        cpu.PC = (hi << 8) | lo;

        cpu.cycles = 7;
    }
}

void Nmi() {
    Write(ABS_SP(cpu.SP--), (cpu.PC >> 8) & 0x00FF);
    Write(ABS_SP(cpu.SP--), cpu.PC & 0x00FF);

    CpuSetFlag(B, 0);
    CpuSetFlag(U, 1);
    CpuSetFlag(I, 1);
    Write(ABS_SP(cpu.SP--), cpu.status);

    cpu.addr_abs = 0xFFFA;
    uint16_t lo = Read(cpu.addr_abs + 0);
    uint16_t hi = Read(cpu.addr_abs + 1);
    cpu.PC = (hi << 8) | lo;

    cpu.cycles = 8;
}


void Clock() {
    if (cpu.cycles == 0) {
        cpu.opcode = Read(cpu.PC);
        CpuSetFlag(U, true);
        cpu.PC++;

        CpuInstruction instruction = LOOKUP[cpu.opcode];
        cpu.cycles = instruction.cycles;
        uint8_t addrmode_additional_cycle = instruction.addrmode();
        uint8_t operate_additional_cycle = instruction.operate();
        cpu.cycles += addrmode_additional_cycle & operate_additional_cycle;
        CpuSetFlag(U, true);
    }
    cpu.cycles--;
}

uint8_t CpuFetch() {
    if (LOOKUP[cpu.opcode].addrmode != IMP) {
        cpu.fetched = Read(cpu.addr_abs);
    }
    return cpu.fetched;
}

bool CpuComplete() {
    return cpu.cycles == 0;
}

Cpu6502 *CpuGet() {
    return &cpu;
}

// Addressing modes

uint8_t IMP() {
    cpu.fetched = cpu.A;
    return 0;
}

uint8_t IMM() {
    cpu.addr_abs = cpu.PC++;
    return 0;
}

uint8_t ZP0() {
    cpu.addr_abs = Read(cpu.PC++);
    cpu.addr_abs &= 0x00FF;
    return 0;
}

uint8_t ZPX() {
    cpu.addr_abs = Read(cpu.PC++) + cpu.X;
    cpu.addr_abs &= 0x00FF;
    return 0;
}

uint8_t ZPY() {
    cpu.addr_abs = Read(cpu.PC++) + cpu.Y;
    cpu.addr_abs &= 0x00FF;
    return 0;
}

uint8_t REL() {
    cpu.addr_rel = Read(cpu.PC++);
    if (cpu.addr_rel & 0x80) {
        cpu.addr_rel |= 0xFF00;
    }
    return 0;
}

uint8_t ABS() {
    uint16_t lo = Read(cpu.PC++);
    uint16_t hi = Read(cpu.PC++);

    cpu.addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t ABX() {
    uint16_t lo = Read(cpu.PC++);
    uint16_t hi = Read(cpu.PC++);

    cpu.addr_abs = ((hi << 8) | lo) + cpu.X;

    if ((cpu.addr_abs & 0xFF00) != (hi << 8)) {
        return 1;
    }

    return 0;
}

uint8_t ABY() {
    uint16_t lo = Read(cpu.PC++);
    uint16_t hi = Read(cpu.PC++);

    cpu.addr_abs = ((hi << 8) | lo) + cpu.Y;

    if ((cpu.addr_abs & 0xFF00) != (hi << 8)) {
        return 1;
    }

    return 0;
}

uint8_t IND() {
    uint16_t ptr_lo = Read(cpu.PC++);
    uint16_t ptr_hi = Read(cpu.PC++);

    uint16_t ptr_addr = (ptr_hi << 8) | ptr_lo;

    //simulate page boundary hardware bug
    if (ptr_lo == 0x00FF) {
        cpu.addr_abs = (Read(ptr_addr & 0xFF00) << 8) | Read(ptr_addr + 0);
    }
    //Behave normally
    else {
        cpu.addr_abs = (Read(ptr_addr + 1) << 8) | Read(ptr_addr + 0);
    }
    return 0;
}

uint8_t IZX() {
    uint16_t t = Read(cpu.PC++);
    uint16_t lo = Read((uint16_t)(t + (uint16_t)cpu.X) & 0x00FF);
    uint16_t hi = Read((uint16_t)(t + (uint16_t)cpu.X + 1) & 0x00FF);
    cpu.addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t IZY() {
    uint16_t t = Read(cpu.PC++);
    uint16_t lo = Read(t & 0x00FF);
    uint16_t hi = Read((t + 1) & 0x00FF);
    cpu.addr_abs = ((hi << 8) | lo) + cpu.Y;
    if ((cpu.addr_abs & 0xFF00) != (hi << 8)) {
        return 1;
    }
    return 0;
}

// Op codes

uint8_t ADC() {
    CpuFetch();
    cpu.temp = (uint16_t)cpu.A + (uint16_t)cpu.fetched + (uint16_t)CpuGetFlag(C);
    CpuSetFlag(C, cpu.temp > 255);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0);
    CpuSetFlag(V, (~((uint16_t)cpu.A ^ (uint16_t)cpu.fetched) & ((uint16_t)cpu.A ^ (uint16_t)cpu.temp)) & 0x0080);
    CpuSetFlag(N, cpu.temp & 0x80);

    cpu.A = cpu.temp & 0x00FF;
    return 1;
}

uint8_t SBC() {
    CpuFetch();
    uint16_t inverted = ((uint16_t)(cpu.fetched)) ^ 0x00FF;
    cpu.temp = (uint16_t)cpu.A + inverted + (uint16_t)CpuGetFlag(C);
    CpuSetFlag(C, cpu.temp & 0xFF00);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0);
    CpuSetFlag(V, (cpu.temp ^ (uint16_t)cpu.A) & (cpu.temp ^ inverted) & 0x0080);
    CpuSetFlag(N, cpu.temp & 0x0080);
    cpu.A = cpu.temp & 0x00FF;
    return 1;
}

uint8_t AND() {
    CpuFetch();
    cpu.A &= cpu.fetched;
    CpuSetFlag(Z, cpu.A == 0x00);
    CpuSetFlag(N, cpu.A & 0x80);
    return 1;
}

uint8_t ASL() {
    CpuFetch();
    cpu.temp = (uint16_t)cpu.fetched << 1;
    CpuSetFlag(C, (cpu.temp & 0xFF00) > 0);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x00);
    CpuSetFlag(N, cpu.temp & 0x80);
    if (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        Write(cpu.addr_abs, cpu.temp & 0x00FF);
    return 0;
}

uint8_t BCC() {
    if (CpuGetFlag(C) == 0) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BCS() {
    if (CpuGetFlag(C) == 1) {
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
    if (CpuGetFlag(Z) == 1) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BIT() {
    CpuFetch();
    cpu.temp = cpu.A & cpu.fetched;
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0);
    CpuSetFlag(N, (cpu.fetched & N));
    CpuSetFlag(V, (cpu.fetched & V));
    return 0;
}

uint8_t BMI() {
    if (CpuGetFlag(N) == 1) {
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
    if (CpuGetFlag(Z) == 0) {
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
    if (CpuGetFlag(N) == 0) {
        cpu.cycles++;
        cpu.addr_abs = cpu.PC + cpu.addr_rel;
        if ((cpu.addr_abs & 0xFF00) != (cpu.PC & 0xFF00)) {
            cpu.cycles++;
        }
        cpu.PC = cpu.addr_abs;
    }
    return 0;
}

uint8_t BRK() {
    cpu.PC++;
    CpuSetFlag(I, 1);
    Write(ABS_SP(cpu.SP--), (cpu.PC >> 8) & 0x00FF);
    Write(ABS_SP(cpu.SP--), cpu.PC & 0x00FF);

    CpuSetFlag(B, 1);
    Write(ABS_SP(cpu.SP--), cpu.status);
    CpuSetFlag(B, 0);

    cpu.PC = (uint16_t)Read(0xFFFE) | ((uint16_t)Read(0xFFFF)) << 8;
    return 0;
}

uint8_t BVC() {
    if (CpuGetFlag(V) == 0) {
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
    if (CpuGetFlag(V) == 1) {
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
    CpuSetFlag(C, false);
    return 0;
}

uint8_t CLD() {
    CpuSetFlag(D, false);
    return 0;
}

uint8_t CLI() {
    CpuSetFlag(I, false);
    return 0;
}

uint8_t CLV() {
    CpuSetFlag(V, false);
    return 0;
}

uint8_t CMP() {
    CpuFetch();
    cpu.temp = (uint16_t)cpu.A - (uint16_t)cpu.fetched;
    CpuSetFlag(C, cpu.A >= cpu.fetched);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x0000);
    CpuSetFlag(N, cpu.temp & 0x0080);
    return 1;
}

uint8_t CPX() {
    CpuFetch();
    cpu.temp = (uint16_t)cpu.X - (uint16_t)cpu.fetched;
    CpuSetFlag(C, cpu.X >= cpu.fetched);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x0000);
    CpuSetFlag(N, cpu.temp & 0x0080);
    return 0;
}

uint8_t CPY() {
    CpuFetch();
    cpu.temp = (uint16_t)cpu.Y - (uint16_t)cpu.fetched;
    CpuSetFlag(C, cpu.Y >= cpu.fetched);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x0000);
    CpuSetFlag(N, cpu.temp & 0x0080);
    return 0;
}

uint8_t DEC() {
    CpuFetch();
    cpu.temp = cpu.fetched - 1;
    Write(cpu.addr_abs, cpu.temp & 0x00FF);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x00);
    CpuSetFlag(N, cpu.temp & 0x80);
    return 0;
}

uint8_t DEX() {
    cpu.X--;
    CpuSetFlag(Z, cpu.X == 0x00);
    CpuSetFlag(N, cpu.X & 0x80);
    return 0;
}

uint8_t DEY() {
    cpu.Y--;
    CpuSetFlag(Z, cpu.Y == 0x00);
    CpuSetFlag(N, cpu.Y & 0x80);
    return 0;
}
uint8_t EOR() {
    CpuFetch();
    cpu.A ^= cpu.fetched;
    CpuSetFlag(Z, cpu.A == 0x00);
    CpuSetFlag(N, cpu.A & 0x80);
    return 1;
}

uint8_t INC() {
    CpuFetch();
    cpu.temp = cpu.fetched + 1;
    Write(cpu.addr_abs, cpu.temp & 0x00FF);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x0000);
    CpuSetFlag(N, cpu.temp & 0x0080);
    return 0;
}

uint8_t INX() {
    cpu.X++;
    CpuSetFlag(Z, cpu.X == 0x00);
    CpuSetFlag(N, cpu.X & 0x80);
    return 0;
}

uint8_t INY() {
    cpu.Y++;
    CpuSetFlag(Z, cpu.Y == 0x00);
    CpuSetFlag(N, cpu.Y & 0x80);
    return 0;
}

uint8_t JMP() {
    cpu.PC = cpu.addr_abs;
    return 0;
}

uint8_t JSR() {
    --cpu.PC;
    Write(ABS_SP(cpu.SP--), (cpu.PC >> 8) & 0x00FF);
    Write(ABS_SP(cpu.SP--), cpu.PC & 0x00FF);

    cpu.PC = cpu.addr_abs;
    return 0;
}

uint8_t LDA() {
    CpuFetch();
    cpu.A = cpu.fetched;
    CpuSetFlag(Z, cpu.A == 0x00);
    CpuSetFlag(N, cpu.A & 0x80);
    return 1;
}

uint8_t LDX() {
    CpuFetch();
    cpu.X = cpu.fetched;
    CpuSetFlag(Z, cpu.X == 0x00);
    CpuSetFlag(N, cpu.X & 0x80);
    return 1;
}

uint8_t LDY() {
    CpuFetch();
    cpu.Y = cpu.fetched;
    CpuSetFlag(Z, cpu.Y == 0x00);
    CpuSetFlag(N, cpu.Y & 0x80);
    return 1;
}

uint8_t LSR() {
    CpuFetch();
    CpuSetFlag(C, cpu.fetched & 0x0001);
    cpu.temp = cpu.fetched >> 1;
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x0000);
    CpuSetFlag(N, cpu.temp & 0x0080);
    if (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        Write(cpu.addr_abs, cpu.temp & 0x00FF);
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
    CpuFetch();
    cpu.A |= cpu.fetched;
    CpuSetFlag(Z, cpu.A == 0x00);
    CpuSetFlag(N, cpu.A & 0x80);
    return 1;
}

uint8_t PHA() {
    Write(ABS_SP(cpu.SP--), cpu.A);
    return 0;
}

uint8_t PHP() {
    Write(ABS_SP(cpu.SP--), cpu.status | B | U);
    CpuSetFlag(B, 0);
    CpuSetFlag(U, 0);
    return 0;
}

uint8_t PLA() {
    cpu.A = Read(ABS_SP(++cpu.SP));
    CpuSetFlag(Z, cpu.A == 0x00);
    CpuSetFlag(N, cpu.A & 0x80);
    return 0;
}

uint8_t PLP() {
    cpu.status = Read(ABS_SP(++cpu.SP));
    CpuSetFlag(U, 1);
    return 0;
}

uint8_t ROL() {
    CpuFetch();
    cpu.temp = (uint16_t)(cpu.fetched << C) | CpuGetFlag(C);
    CpuSetFlag(C, cpu.temp & 0xFF00);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x0000);
    CpuSetFlag(N, cpu.temp & 0x0080);
    if (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        Write(cpu.addr_abs, cpu.temp & 0x00FF);
    return 0;
}

uint8_t ROR() {
    CpuFetch();
    cpu.temp = (uint16_t)(CpuGetFlag(C) << 7) | (cpu.fetched >> C);
    CpuSetFlag(C, cpu.fetched & 0x01);
    CpuSetFlag(Z, (cpu.temp & 0x00FF) == 0x00);
    CpuSetFlag(N, cpu.temp & 0x0080);
    if (LOOKUP[cpu.opcode].addrmode == IMP)
        cpu.A = cpu.temp & 0x00FF;
    else
        Write(cpu.addr_abs, cpu.temp & 0x00FF);
    return 0;
}

uint8_t RTI() {
    cpu.status = Read(ABS_SP(++cpu.SP));
    CpuSetFlag(B, ~CpuGetFlag(B));
    CpuSetFlag(U, ~CpuGetFlag(U));
    uint16_t pc_lo = Read(ABS_SP(++cpu.SP));
    uint16_t pc_hi = Read(ABS_SP(++cpu.SP));
    cpu.PC = (pc_hi << 8) | pc_lo;
    return 0;
}

uint8_t RTS() {
    uint16_t pc_lo = Read(ABS_SP(++cpu.SP));
    uint16_t pc_hi = Read(ABS_SP(++cpu.SP));
    cpu.PC = (pc_hi << 8) | pc_lo;
    cpu.PC++;
    return 0;
}

uint8_t SEC() {
    CpuSetFlag(C, 1);
    return 0;
}

uint8_t SED() {
    CpuSetFlag(D, 1);
    return 0;
}

uint8_t SEI() {
    CpuSetFlag(I, 1);
    return 0;
}

uint8_t STA() {
    Write(cpu.addr_abs, cpu.A);
    return 0;
}

uint8_t STX() {
    Write(cpu.addr_abs, cpu.X);
    return 0;
}

uint8_t STY() {
    Write(cpu.addr_abs, cpu.Y);
    return 0;
}

uint8_t TAX() {
    cpu.X = cpu.A;
    CpuSetFlag(Z, cpu.X == 0x00);
    CpuSetFlag(N, cpu.X & 0x80);
    return 0;
}

uint8_t TAY() {
    cpu.Y = cpu.A;
    CpuSetFlag(Z, cpu.Y == 0x00);
    CpuSetFlag(N, cpu.Y & 0x80);
    return 0;
}

uint8_t TSX() {
    cpu.X = cpu.SP;
    CpuSetFlag(Z, cpu.X == 0x00);
    CpuSetFlag(N, cpu.X & 0x80);
    return 0;
}

uint8_t TXA() {
    cpu.A = cpu.X;
    CpuSetFlag(Z, cpu.A == 0x00);
    CpuSetFlag(N, cpu.A & 0x80);
    return 0;
}

uint8_t TXS() {
    cpu.SP = cpu.X;
    return 0;
}

uint8_t TYA() {
    cpu.A = cpu.Y;
    CpuSetFlag(Z, cpu.A == 0x00);
    CpuSetFlag(N, cpu.A & 0x80);
    return 0;
}

uint8_t XXX() {
    return 0;
}