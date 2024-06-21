#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <bitset>
#include "Cpu.h"
#include "Bus.h"

using json = nlohmann::json;

json load_json(const std::string &filename)
{
    std::ifstream file(filename);
    json j;
    if (file.is_open())
    {
        file >> j;
    }
    return j;
}

uint8_t storeProcessorStatus(CPURegisters registers)
{
    uint8_t flags = 0;
    flags |= registers.carryFlag << 0;
    flags |= registers.zeroFlag << 1;
    flags |= registers.interruptDisableFlag << 2;
    flags |= registers.decimalModeFlag << 3;
    flags |= registers.breakCommandFlag << 4; // Set the break flag correctly
    flags |= 1 << 5; // This bit has no use but is always set to 1
    flags |= registers.overflowFlag << 6;
    flags |= registers.negativeFlag << 7;

    return flags;
}


void loadProcessorStatus(CPURegisters &registers, int p)
{
    // Read the registers back into their respective flags.
    registers.carryFlag = (p >> 0) & 1;
    registers.zeroFlag = (p >> 1) & 1;
    registers.interruptDisableFlag = (p >> 2) & 1;
    registers.decimalModeFlag = (p >> 3) & 1;
    registers.breakCommandFlag = (p >> 4) & 1; // Load the break flag correctly
    registers.overflowFlag = (p >> 6) & 1;
    registers.negativeFlag = (p >> 7) & 1;
}


std::string formatRegisters(const CPURegisters &registersBeforeRun, const CPURegisters &registers, json &test) {
    std::stringstream ss;

    ss << std::endl
        << "A: " << std::hex << (int) registersBeforeRun.A << " -> " << std::hex << (int) registers.A << " | " << std::hex << (int) test["final"]["a"].get<uint8_t>() << std::endl
        << "X: " << std::hex << (int) registersBeforeRun.X << " -> " << std::hex << (int) registers.X << " | " << std::hex << (int) test["final"]["x"].get<uint8_t>() << std::endl
        << "Y: " << std::hex << (int) registersBeforeRun.Y << " -> " << std::hex << (int) registers.Y << " | " << std::hex << (int) test["final"]["y"].get<uint8_t>() << std::endl
        << "PC: " << std::hex << (int) registersBeforeRun.PC << " -> " << std::hex << (int) registers.PC << " | " << std::hex << (int) test["final"]["pc"].get<uint16_t>() << std::endl
        << "SP: " << std::hex << (int) registersBeforeRun.SP << " -> " << std::hex << (int) registers.SP << " | " << std::hex << (int) test["final"]["s"].get<uint8_t>() << std::endl
        << "Flags: " << std::bitset<8>(storeProcessorStatus(registersBeforeRun)) << " -> " << std::bitset<8>(storeProcessorStatus(registers)) << " | " << std::bitset<8>(test["final"]["p"].get<uint8_t>()) << std::endl;
        
    // Add other registers in the same way...
    return ss.str();
}


class CpuTest : public ::testing::TestWithParam<std::string>
{
protected:
    CPURegisters registers;
    Bus bus;
    CPU *cpu;

    void SetUp() override
    {
        // Allocate the needed 64KB of PRG memory to run the test
        // bus.allocatePRGMemory(4);
        cpu = new CPU(bus);
    }

    void TearDown() override
    {
        delete cpu;
    }
};

TEST_P(CpuTest, TestCpuRegisters)
{
    std::string fileName = GetParam();
    json tests = load_json("ProcessorTests/" + fileName);

    for (json &test : tests)
    {
        cpu->reset();
        registers.A = test["initial"]["a"].get<uint8_t>(); // Use lowercase and add get<uint8_t>()
        registers.X = test["initial"]["x"].get<uint8_t>();
        registers.Y = test["initial"]["y"].get<uint8_t>();
        loadProcessorStatus(registers, test["initial"]["p"].get<int>()); // Ensure correct type
        registers.PC = test["initial"]["pc"].get<uint16_t>();
        registers.SP = test["initial"]["s"].get<uint8_t>();

        CPURegisters registersBeforeRun = registers;

        // Write the initial memory values for test
        for (const auto &item : test["initial"]["ram"])
        {
            uint16_t address = item[0].get<uint16_t>();
            uint8_t value = item[1].get<uint8_t>();
            bus.writeMemory(address, value);
        }

        cpu->setRegisters(registers);
        cpu->run(test["cycles"].size()); // Execute one cycle to set perform the operation

        registers = cpu->getRegisters();

        ASSERT_EQ(registers.A, test["final"]["a"].get<uint8_t>()) << formatRegisters(registersBeforeRun, registers, test);
        ASSERT_EQ(registers.X, test["final"]["x"].get<uint8_t>()) << formatRegisters(registersBeforeRun, registers, test);
        ASSERT_EQ(registers.Y, test["final"]["y"].get<uint8_t>()) << formatRegisters(registersBeforeRun, registers, test);
        // Ensure the processor status is compared correctly
        ASSERT_EQ(storeProcessorStatus(registers), test["final"]["p"].get<uint8_t>()) << formatRegisters(registersBeforeRun, registers, test);
        ASSERT_EQ(registers.PC, test["final"]["pc"].get<uint16_t>()) << formatRegisters(registersBeforeRun, registers, test);
        ASSERT_EQ(registers.SP, test["final"]["s"].get<uint8_t>()) << formatRegisters(registersBeforeRun, registers, test);
    }
};

std::vector<std::string> generateTestFileNames()
{
    std::vector<std::string> fileNames;

    for (int i = 0; i <= 0xFF; ++i)
    {
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << std::hex << i;
        fileNames.push_back(ss.str() + ".json");
    }
    return fileNames;
}

INSTANTIATE_TEST_SUITE_P(
    ProcessorTests,
    CpuTest,
    ::testing::ValuesIn(generateTestFileNames()));