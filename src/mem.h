#pragma once

#define MEM_SIZE 1024*1024*128 // 128 mib
#define MEM_BASE 0x80000000

#include <vector>
#include <cstdint>

class Memory {
public:
    std::vector<uint8_t> memory;
    Memory(std::vector<uint8_t> binary);
    uint64_t load(uint64_t addr, uint64_t size);
    uint64_t load8(uint64_t addr);
    uint64_t load16(uint64_t addr);
    uint64_t load32(uint64_t addr);
    uint64_t load64(uint64_t addr);

    void store(uint64_t addr, uint64_t size, uint64_t value);
    void store8(uint64_t addr, uint64_t value);
    void store16(uint64_t addr, uint64_t value);
    void store32(uint64_t addr, uint64_t value);
    void store64(uint64_t addr, uint64_t value);
};