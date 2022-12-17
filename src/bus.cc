#include <vector>
#include <iostream>
#include "bus.h"
#include "memory.h"

/*
#define MEMORY_BASE 0x80000000
#define MEMORY_SIZE 1024*1024*128
*/

Bus::Bus(std::vector<uint8_t> binary): memory(binary) {
};

uint64_t Bus::load(uint64_t addr, uint64_t size) {
    if (MEM_BASE <= addr) {
        return memory.load(addr, size);
    }
    return -1;
}

void Bus::store(uint64_t addr, uint64_t size, uint64_t value) {
    if (MEM_BASE <= addr) {
        memory.store(addr, size, value);
    }
}