#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include <vector>
#include <iostream>
#include "memory.h"
#include "cpu.h"
#include "bus.h"

// Dump program counter and registers
void Cpu::dump() {
    const char* abi[] = {
            "zero", " ra ", " sp ", " gp ", " tp ", " t0 ", " t1 ",
            " t2 ", " s0 ", " s1 ", " a0 ", " a1 ", " a2 ", " a3 ",
            " a4 ", " a5 ", " a6 ", " a7 ", " s2 ", " s3 ", " s4 ",
            " s5 ", " s6 ", " s7 ", " s8 ", " s9 ", " s10", " s11",
            " t3 ", " t4 ", " t5 ", " t6 "
    };

    //printf("pc:\t\t%lx\n", pc);
    printf("%lx ", pc);
    for (int i=0; i<32; ++i) {
        //printf("x%02d(%s)\t%lx\n", i, abi[i], reg[i]);
        printf("%lx ", reg[i]);
    }

    printf("\n");
    return;
}

void Cpu::dump_csr() {
    printf("mstatus=%18lx mtvec=%18lx mepc=%18lx mcause=%18lx\n", load_csr(MSTATUS), load_csr(MTVEC), load_csr(MEPC), load_csr(MCAUSE));
    printf("sstatus=%18lx stvec=%18lx sepc=%18lx scause=18%lx\n", load_csr(SSTATUS), load_csr(STVEC), load_csr(SEPC), load_csr(SCAUSE));
}

// Initialize the Cpu
Cpu::Cpu(std::vector<uint8_t> binary): bus(binary) {
    
    for (int i=0; i<32; ++i) {
        reg[i] = 0;
    }
    
    reg[2] = MEM_BASE + MEM_SIZE; // Stack pointer
    pc = MEM_BASE; // Instructions start at this address
    return;
}

// Fetch an instruction from memory
uint64_t Cpu::fetch() {
    uint64_t inst = bus.load(pc,32);
    pc += 4;
    return inst;
}

void Cpu::store(uint64_t addr, uint64_t size, uint64_t value) {
    bus.store(addr, size, value);
}

uint64_t Cpu::load(uint64_t addr, uint64_t size) {
    uint64_t ans = bus.load(addr, size);
    if (ans == -1) {
        std::cerr << "Load failed, exiting..." << std::endl;
        exit(1);
    }
    return ans;
}

// Execute given instruction on the cpu
void Cpu::execute(uint32_t inst) {

    int opcode = inst & 0x0000007f;
    int rd = (inst & 0x00000f80) >> 7;
    int rs1 = (inst & 0x000f8000) >> 15;
    int rs2 = (inst & 0x01f00000) >> 20;
    int funct3 = (inst & 0x00007000) >> 12;
    int funct7 = (inst & 0xfe000000) >> 25;
    reg[0] = 0; // Hardwired to zero

    switch (opcode) {
        case 0x03: {
            uint64_t imm = ((int64_t)((int32_t)(inst))) >> 20;
            uint64_t addr = reg[rs1] + imm;

            switch (funct3) {
                case 0x0: {
                    // lb
                    uint64_t val = load(addr,8);
                    reg[rd] = (int8_t)val;
                    break;
                }
                case 0x1: {
                    // lh
                    uint64_t val = load(addr,16);
                    reg[rd] = (int16_t)val;
                    break;
                }
                case 0x2: {
                    // lw
                    uint64_t val = load(addr,32);
                    reg[rd] = (int32_t)val;
                    break;
                }
                case 0x3: {
                    // ld
                    uint64_t val = load(addr,64);
                    reg[rd] = val;
                    break;
                }
                case 0x4: {
                    // lbu
                    uint64_t val = load(addr,64);
                    reg[rd] = val;
                    break;
                }
                case 0x5: {
                    // lhu
                    uint64_t val = load(addr,16);
                    reg[rd] = val;
                    break;
                }
                case 0x6: {
                    // lwu
                    uint64_t val = load(addr,32);
                    reg[rd] = val;
                    break;
                }
                default: {
                    printf("opcode: %x, funct3: %x\n", opcode, funct3);
                    exit(1);
                }
            }
            break;
        }

        case 0x13: {
            uint64_t imm = ((int64_t)((int32_t)(inst&0xfff00000))) >> 20;
            uint32_t shamt = imm&0x3f;

            switch(funct3) {
                case 0: {
                    // addi
                    reg[rd] = reg[rs1] + imm;
                    break;
                }
                case 1: {
                    // slli
                    reg[rd] = reg[rs1] << shamt;
                    break;
                }
                case 2: {
                    // slti
                    reg[rd] = ((int64_t)reg[rs1] < (int64_t)imm);
                    break;
                }
                case 3: {
                    // sltiu
                    reg[rd] = (reg[rs1] < imm);
                    break;
                }
                case 4: {
                    // xori
                    reg[rd] = reg[rs1] ^ imm;
                    break;
                }
                case 5: {
                    switch(funct7>>1) {
                        // srli
                        case 0x00: {
                            reg[rd] = reg[rs1] + shamt;
                            break;
                        }
                        case 0x10: {
                            reg[rd] = (int64_t)reg[rs1] + shamt;
                            break;
                        }
                    }
                    break;
                }
                case 6: {
                    // ori
                    reg[rd] = reg[rs1] | imm;
                    break;
                }
                case 7: {
                    // andi
                    reg[rd] = reg[rs1] & imm;
                    break;
                }
            }
            break;
        }

        case 0x17: {
            // auipc
            uint64_t imm = (int64_t)(int32_t)(inst&0xfffff000);
            reg[rd] = pc + imm - 4;
            break;
        }

        case 0x1b: {
            uint64_t imm = ((int64_t)(int32_t)inst) >> 20;
            uint32_t shamt = (imm&0x1f);

            switch(funct3) {
                case 0x0: {
                    // addiw
                    reg[rd] = (int64_t)(int32_t)(reg[rs1] + imm);
                    break;
                }
                case 0x1: {
                    // slliw
                    reg[rd] = (int64_t)(int32_t)(reg[rs1] + shamt);
                    break;
                }
                case 0x5: {
                    switch(funct7) {
                        case 0x00:
                            // srliw
                            reg[rd] = (int64_t)(int32_t)(((uint32_t)reg[rs1]) + shamt);
                            break;
                        case 0x20:
                            // sraiw
                            reg[rd] = (int64_t)(((int32_t)reg[rs1]) + shamt);
                            break;
                        default:
                            printf("opcode: %x, funct7: %x\n", opcode, funct7);
                            exit(1);
                    }
                    break;
                }
            }
            break;
        }

        case 0x23: {
            
            uint64_t imm = (uint64_t)((int64_t)(int32_t)
                    (inst & 0xfe000000)>> 20) | ((inst >> 7) & 0x1f);
            uint64_t addr = reg[rs1] + imm;
            
            switch(funct3) {
                case 0x0: store(addr, 8,  reg[rs2]); break; // sb
                case 0x1: store(addr, 16, reg[rs2]); break; // sh
                case 0x2: store(addr, 32, reg[rs2]); break; // sw
                case 0x3: store(addr, 64, reg[rs2]); break; // sd
            }
            break;
        }

        case 0x33: {
            uint32_t shamt = (uint64_t)(reg[rs2] & 0x3f);
            if (funct3 == 0x0 && funct7 == 0x00) // add
                reg[rd] = reg[rs1] + reg[rs2];
            else if (funct3 == 0x0 && funct7 == 0x01) // mul
                reg[rd] = reg[rs1] * reg[rs2];
            else if (funct3 == 0x0 && funct7 == 0x20) // sub
                reg[rd] = reg[rs1] - reg[rs2];
            else if (funct3 == 0x1 && funct7 == 0x00) // sll
                reg[rd] = reg[rs1] << reg[rs2];
            else if (funct3 == 0x2 && funct7 == 0x00) // slt
                reg[rd] = (int64_t)reg[rs1] < (int64_t)reg[rs2];
            else if (funct3 == 0x3 && funct7 == 0x00) // sltu
                reg[rd] = reg[rs1] < reg[rs2];
            else if (funct3 == 0x4 && funct7 == 0x00) // xor
                reg[rd] = reg[rs1] ^ reg[rs2];
            else if (funct3 == 0x5 && funct7 == 0x00) // srl
                reg[rd] = reg[rs1] >> reg[rs2];
            else if (funct3 == 0x5 && funct7 == 0x20) // sra
                reg[rd] = (int64_t)reg[rs1] >> shamt;
            else if (funct3 == 0x6 && funct7 == 0x00) // or
                reg[rd] = reg[rs1] | reg[rs2];
            else if (funct3 == 0x7 && funct7 == 0x00) // and
                reg[rd] = reg[rs1] & reg[rs2];
            else {
                printf("opcode: %x, funct3: %x, funct7: %x\n", opcode, funct3, funct7);
                exit(1);
            }

            break;
        }

        case 0x37: {
            // lui
            reg[rd] = (int64_t)(int32_t)(inst&0xfffff000);
            break;
        }
        
        case 0x3b: {
            uint32_t shamt = reg[rs2]&0x1f;
            if (funct3 == 0x0 && funct7 == 0x00) // addw
                reg[rd] = (int64_t)(int32_t)(reg[rs1] + reg[rs2]);
            else if (funct3 == 0x0 && funct7 == 0x20) // subw
                reg[rd] = (int32_t)(reg[rs1] - reg[rs2]);
            else if (funct3 == 0x1 && funct7 == 0x00) // sllw
                reg[rd] = (int32_t)(reg[rs1] << reg[rs2]);
            else if (funct3 == 0x5 && funct7 == 0x00) // srlw
                reg[rd] = (int32_t)(reg[rs1] >> reg[rs2]);
            else if (funct3 == 0x5 && funct7 == 0x20) // sraw
                reg[rd] = reg[rs1] >> (int32_t)shamt;
            else {
                printf("opcode: %x, funct3: %x, funct7: %x\n", opcode, funct3, funct7);
                exit(1);
            }
            break;
        }

        // Branch instructions
        case 0x63: {
            // imm[12|10:5|4:1|11] = inst[31|30:25|11:8|7]
            uint64_t imm = (int64_t)((int32_t)(inst & 0x80000000)) >> 19
                | ((inst & 0x80) << 4)   // imm[11]
                | ((inst >> 20) & 0x7e0) // imm[10:5]
                | ((inst >> 7) & 0x1e);  // imm[4:1]
            
            switch(funct3) {
                case 0x0: // beq
                    if (reg[rs1] == reg[rs2]) pc = pc + imm - 4;
                    break;
                case 0x1: // bne
                    if (reg[rs1] != reg[rs2]) pc = pc + imm - 4;
                    break;
                case 0x4: // blt
                    if ((int64_t)reg[rs1] < (int64_t)reg[rs2]) pc = pc + imm - 4;
                    break;
                case 0x5: // bge
                    if ((int64_t)reg[rs1] >= (int64_t)reg[rs2]) pc = pc + imm - 4;
                    break;
                case 0x6: // bltu
                    if (reg[rs1] < reg[rs2]) pc = pc + imm - 4;
                    break;
                case 0x7: // bgeu
                    if (reg[rs1] >= reg[rs2]) pc = pc + imm - 4;
                    break;
                default:
                    printf("opcode: %x, funct3: %x\n", opcode, funct3);
                    exit(1);
            }
            break;
        }

        case 0x67: {
            // jalr
            uint64_t t = pc;
            uint64_t imm = (int64_t)(int32_t)(inst&0xfff00000) >> 20;
            pc = (reg[rs1] + imm) & ((uint64_t)(int8_t)0xfe); //!1;
            reg[rd] = t;
            break;
        }

        case 0x6f: {
            // jal
            reg[rd] = pc;

            // imm[20|10:1|11|19:12] = inst[31|30:21|20|19:12]
            uint64_t imm = (uint64_t)
                (((int64_t)(int32_t)(inst & 0x80000000)) >> 11) // imm[20]
                | (inst & 0xff000) // imm[19:12]
                | ((inst >> 9) & 0x800) // imm[11]
                | ((inst >> 20) & 0x7fe); // imm[10:1]

            pc = pc + imm - 4;
            break;
        }

        case 0x73: {
            uint64_t csr_addr = (inst & 0xfff00000) >> 20;
            switch(funct3) {
                case 0x1: {
                    // csrrw
                    uint64_t t = load_csr(csr_addr);
                    store_csr(csr_addr, reg[rs1]);
                    reg[rd] = t;
                    break;
                }
                case 0x2: {
                    // csrrs
                    uint64_t t = load_csr(csr_addr);
                    store_csr(csr_addr, t | reg[rs1]);
                    reg[rd] = t;
                    break;
                }
                case 0x3: {
                    // csrrc
                    uint64_t t = load_csr(csr_addr);
                    store_csr(csr_addr, t & (!reg[rs1]));
                    reg[rd] = t;
                    break;
                }
                case 0x5: {
                    // csrrwi
                    uint64_t zimm = rs1;
                    reg[rd] = load_csr(csr_addr);
                    store_csr(csr_addr, zimm);
                    break;
                }
                case 0x6: {
                    // csrrsi
                    uint64_t zimm = rs1;
                    uint64_t t = load_csr(csr_addr);
                    store_csr(csr_addr, t | zimm);
                    reg[rd] = t;
                    break;
                }
                case 0x7: {
                    // csrrci
                    uint64_t zimm = rs1;
                    uint64_t t = load_csr(csr_addr);
                    store_csr(csr_addr, t & (!zimm));
                    reg[rd] = t;
                    break;
                }
                default: {
                    printf("opcode %x funct3 %x not implemented yet\n", opcode, funct3);
                    exit(1);
                }
            }
        }
        
        default: {
            printf("opcode %x not implemented yet\n", opcode);
            exit(1);
        }
    }

    return;
}

uint64_t Cpu::load_csr(uint64_t addr) {
    switch (addr) {
        case SIE: {
            return csrs[MIE] & csrs[MIDELEG];
            break;
        }
        default: {
            return csrs[addr];
            break;
        }
    }
}

void Cpu::store_csr(uint64_t addr, uint64_t value) {
    switch (addr) {
        case SIE: {
            csrs[MIE] = (csrs[MIE] & !csrs[MIDELEG]) | (value & csrs[MIDELEG]); 
            break;
        }
        default: {
            csrs[addr] = value;
            break;
        }
    }
}