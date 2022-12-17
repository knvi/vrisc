#pragma once

#include <vector>
#include "mem.h"

class Bus {
    Memory memory;

public:
    Bus(std::vector<uint8_t> binary);
    uint64_t load(uint64_t addr, uint64_t size);
    void store(uint64_t addr, uint64_t size, uint64_t value);
};