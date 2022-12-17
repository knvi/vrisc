#include <cstdint>
#include <iostream>
#include "mem.h"

Memory::Memory(std::vector<uint8_t> bin) : memory(MEM_SIZE) {
  for (int i = 0; i < bin.size(); i++) {
    memory[i] = bin[i];
  }
}

uint64_t Memory::load(uint64_t addr, uint64_t size) {
    switch(size) {
        case 8: return load8(addr); break;
        case 16: return load16(addr); break;
        case 32: return load32(addr); break;
        case 64: return load64(addr); break;
    }

    return -1;
}

uint64_t Memory::load8(uint64_t addr) {
    uint64_t index = (addr - MEM_BASE);
    return memory[index];
}

uint64_t Memory::load16(uint64_t addr) {
    uint64_t index = (addr - MEM_BASE);
    return memory[index] | memory[index+1] << 8;
}

uint64_t Memory::load32(uint64_t addr) {
    uint64_t index = (addr - MEM_BASE);
    return memory[index]
             | memory[index+1] << 8
             | memory[index+2] << 16
             | memory[index+3] << 24;
}

uint64_t Memory::load64(uint64_t addr) {
    uint64_t index = (addr - MEM_BASE);
    return (uint64_t) memory[index]
             | (uint64_t) memory[index+1] << 8
             | (uint64_t) memory[index+2] << 16
             | (uint64_t) memory[index+3] << 24
             | (uint64_t) memory[index+4] << 32
             | (uint64_t) memory[index+5] << 40
             | (uint64_t) memory[index+6] << 48
             | (uint64_t) memory[index+7] << 56;
}

void Memory::store(uint64_t addr, uint64_t size, uint64_t value) {
    switch(size) {
        case 8:  return store8(addr, value); break;
        case 16: return store16(addr, value); break;
        case 32: return store32(addr, value); break;
        case 64: return store64(addr, value); break;
    }
}

void Memory::store8(uint64_t addr, uint64_t value) {
    uint64_t index = (addr - MEM_BASE);
    memory[index] = (uint8_t) value;
}

void Memory::store16(uint64_t addr, uint64_t value) {
    uint64_t index = (addr - MEM_BASE);
    memory[index] = (uint8_t) (value&0xff);
    memory[index+1] = (uint8_t) (value>>8)&0xff;
}

void Memory::store32(uint64_t addr, uint64_t value) {
    uint64_t index = (addr - MEM_BASE);
    memory[index] = (uint8_t) (value&0xff);
    memory[index+1] = (uint8_t) (value>>8)&0xff;
    memory[index+2] = (uint8_t) (value>>16)&0xff;
    memory[index+3] = (uint8_t) (value>>24)&0xff;
}

void Memory::store64(uint64_t addr, uint64_t value) {
    uint64_t index = (addr - MEM_BASE); 
    memory[index] = (uint8_t) (value&0xff);
    memory[index+1] = (uint8_t) (value>>8)&0xff;
    memory[index+2] = (uint8_t) (value>>16)&0xff;
    memory[index+3] = (uint8_t) (value>>24)&0xff;
    memory[index+4] = (uint8_t) (value>>32)&0xff;
    memory[index+5] = (uint8_t) (value>>40)&0xff;
    memory[index+6] = (uint8_t) (value>>48)&0xff;
    memory[index+7] = (uint8_t) (value>>56)&0xff;
}