#pragma once
#include "bus.h"

#define MHARTID 0xf14
#define MSTATUS 0x300
#define MEDELEG 0x302
#define MIDELEG 0x303
#define MIE 0x304
#define MTVEC 0x305
#define MCOUNTEREN 0x306
#define MSCRATCH 0x340
#define MEPC 0x341
#define MCAUSE 0x342
#define MTVAL 0x343
#define MIP 0x344

#define SSTATUS 0x100
#define SIE 0x104
#define STVEC 0x105
#define SSCRATCH 0x140
#define SEPC 0x141
#define SCAUSE 0x142
#define STVAL 0x143
#define SIP 0x144
#define SATP 0x180

class Cpu {
public:
    uint64_t pc;
    uint64_t reg[32];
    uint64_t csrs[4096];
    Bus bus;

public:
    Cpu(std::vector<uint8_t> binary);
    uint64_t fetch();
    void execute(uint32_t inst);
    void dump();
    void dump_csr();
    uint64_t load_csr(uint64_t addr);
    void store_csr(uint64_t addr, uint64_t value);

    uint64_t load(uint64_t addr, uint64_t size);
    void store(uint64_t addr, uint64_t size, uint64_t value);
};