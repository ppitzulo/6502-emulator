#include <vector>
#include "Cpu.h"
#include "Bus.h"

Bus::Bus() //: cpu(*this)
{
    ram = new uint8_t[0xFFFF];
    cpu = new CPU(*this);
    // ram = new uint8_t[0xFFFF];
    // ram[0x0801] = 0x0088; not yet sure what this does yet
}

Bus::~Bus()
{
    delete[] ram;
    delete cpu;
}

uint8_t Bus::readMemory(uint16_t address)
{
    return ram[address];
}

void Bus::writeMemory(uint16_t address, uint8_t data)
{
    ram[address] = data;
}

void Bus::clearMemory()
{
    for (int i = 0; i < 0xFFFF; i++)
    {
        ram[i] = 0;
    }
}

void Bus::loadProgram(std::vector<uint8_t> &program, uint16_t startAddress)
{
    for (int i = 0; i < program.size(); i++)
    {
        ram[startAddress + i] = program[i];
    }
    CPURegisters registers = cpu->getRegisters();
    registers.PC = startAddress;
    cpu->setRegisters(registers);
}

void Bus::run()
{
    cpu->run();
}
