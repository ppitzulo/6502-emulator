#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <vector>
#include "Cpu.h"

class Bus
{
public:
    Bus();
    ~Bus();

    uint8_t readMemory(uint16_t address);
    void writeMemory(uint16_t address, uint8_t data);
    void loadProgram(std::vector<uint8_t> &program, uint16_t startAddress);
    void run();
    // int testVector(const std::vector<int> &program);


private:
    uint8_t* ram;
    CPU *cpu;
};


#endif