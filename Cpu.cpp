#include <iomanip>
#include <iostream>
#include <sstream>
#include "Cpu.h"
#include "Bus.h"

#define DEBUG false // TODO: Add ability to set this from the command line

using namespace std;

CPU::CPU(Bus &bus) : bus(bus), opcodes{
    // 0x00 - 0x0F
    {&CPU::implied, &CPU::BRK, "BRK", 7}, {&CPU::indirectX, &CPU::ORA, "ORA", 6}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectX, &CPU::SLO, "SLO", 8},
    {&CPU::zeropage, &CPU::NOP, "NOP", 3}, {&CPU::zeropage, &CPU::ORA, "ORA", 3}, {&CPU::zeropage, &CPU::ASL, "ASL", 5}, {&CPU::zeropage, &CPU::SLO, "SLO", 5},
    {&CPU::implied, &CPU::PHP, "PHP", 3}, {&CPU::immediate, &CPU::ORA, "ORA", 2}, {&CPU::accumulator, &CPU::ASL, "ASL", 2}, {&CPU::immediate, &CPU::ANC, "ANC", 2},
    {&CPU::absolute, &CPU::NOP, "NOP", 4}, {&CPU::absolute, &CPU::ORA, "ORA", 4}, {&CPU::absolute, &CPU::ASL, "ASL", 6}, {&CPU::absolute, &CPU::SLO, "SLO", 6},

    // 0x10 - 0x1F
    {&CPU::relative, &CPU::BPL, "BPL", 2}, {&CPU::indirectY, &CPU::ORA, "ORA", 5}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::SLO, "SLO", 8},
    {&CPU::zeropageX, &CPU::NOP, "NOP", 4}, {&CPU::zeropageX, &CPU::ORA, "ORA", 4}, {&CPU::zeropageX, &CPU::ASL, "ASL", 6}, {&CPU::zeropageX, &CPU::SLO, "SLO", 6},
    {&CPU::implied, &CPU::CLC, "CLC", 2}, {&CPU::absoluteY, &CPU::ORA, "ORA", 4}, {&CPU::implied, &CPU::NOP, "NOP", 2}, {&CPU::absoluteY, &CPU::SLO, "SLO", 7},
    {&CPU::absoluteX, &CPU::NOP, "NOP", 4}, {&CPU::absoluteX, &CPU::ORA, "ORA", 4}, {&CPU::absoluteX, &CPU::ASL, "ASL", 7}, {&CPU::absoluteX, &CPU::SLO, "SLO", 7},

    // 0x20 - 0x2F
    {&CPU::absolute, &CPU::JSR, "JSR", 6}, {&CPU::indirectX, &CPU::AND, "AND", 6}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectX, &CPU::RLA, "RLA", 8},
    {&CPU::zeropage, &CPU::BIT, "BIT", 3}, {&CPU::zeropage, &CPU::AND, "AND", 3}, {&CPU::zeropage, &CPU::ROL, "ROL", 5}, {&CPU::zeropage, &CPU::RLA, "RLA", 5},
    {&CPU::implied, &CPU::PLP, "PLP", 4}, {&CPU::immediate, &CPU::AND, "AND", 2}, {&CPU::accumulator, &CPU::ROL, "ROL", 2}, {&CPU::immediate, &CPU::ANC, "ANC", 2},
    {&CPU::absolute, &CPU::BIT, "BIT", 4}, {&CPU::absolute, &CPU::AND, "AND", 4}, {&CPU::absolute, &CPU::ROL, "ROL", 6}, {&CPU::absolute, &CPU::RLA, "RLA", 6},

    // 0x30 - 0x3F
    {&CPU::relative, &CPU::BMI, "BMI", 2}, {&CPU::indirectY, &CPU::AND, "AND", 5}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::RLA, "RLA", 8},
    {&CPU::zeropageX, &CPU::NOP, "NOP", 4}, {&CPU::zeropageX, &CPU::AND, "AND", 4}, {&CPU::zeropageX, &CPU::ROL, "ROL", 6}, {&CPU::zeropageX, &CPU::RLA, "RLA", 6},
    {&CPU::implied, &CPU::SEC, "SEC", 2}, {&CPU::absoluteY, &CPU::AND, "AND", 4}, {&CPU::implied, &CPU::NOP, "NOP", 2}, {&CPU::absoluteY, &CPU::RLA, "RLA", 7},
    {&CPU::absoluteX, &CPU::NOP, "NOP", 4}, {&CPU::absoluteX, &CPU::AND, "AND", 4}, {&CPU::absoluteX, &CPU::ROL, "ROL", 7}, {&CPU::absoluteX, &CPU::RLA, "RLA", 7},

    // 0x40 - 0x4F
    {&CPU::implied, &CPU::RTI, "RTI", 6}, {&CPU::indirectX, &CPU::EOR, "EOR", 6}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectX, &CPU::SRE, "SRE", 8},
    {&CPU::zeropage, &CPU::NOP, "NOP", 3}, {&CPU::zeropage, &CPU::EOR, "EOR", 3}, {&CPU::zeropage, &CPU::LSR, "LSR", 5}, {&CPU::zeropage, &CPU::SRE, "SRE", 5},
    {&CPU::implied, &CPU::PHA, "PHA", 3}, {&CPU::immediate, &CPU::EOR, "EOR", 2}, {&CPU::accumulator, &CPU::LSR, "LSR", 2}, {&CPU::immediate, &CPU::ALR, "ALR", 2},
    {&CPU::absolute, &CPU::JMP, "JMP", 3}, {&CPU::absolute, &CPU::EOR, "EOR", 4}, {&CPU::absolute, &CPU::LSR, "LSR", 6}, {&CPU::absolute, &CPU::SRE, "SRE", 6},

    // 0x50 - 0x5F
    {&CPU::relative, &CPU::BVC, "BVC", 2}, {&CPU::indirectY, &CPU::EOR, "EOR", 5}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::SRE, "SRE", 8},
    {&CPU::zeropageX, &CPU::NOP, "NOP", 4}, {&CPU::zeropageX, &CPU::EOR, "EOR", 4}, {&CPU::zeropageX, &CPU::LSR, "LSR", 6}, {&CPU::zeropageX, &CPU::SRE, "SRE", 6},
    {&CPU::implied, &CPU::CLI, "CLI", 2}, {&CPU::absoluteY, &CPU::EOR, "EOR", 4}, {&CPU::implied, &CPU::NOP, "NOP", 2}, {&CPU::absoluteY, &CPU::SRE, "SRE", 7},
    {&CPU::absoluteX, &CPU::NOP, "NOP", 4}, {&CPU::absoluteX, &CPU::EOR, "EOR", 4}, {&CPU::absoluteX, &CPU::LSR, "LSR", 7}, {&CPU::absoluteX, &CPU::SRE, "SRE", 7},

    // 0x60 - 0x6F
    {&CPU::implied, &CPU::RTS, "RTS", 6}, {&CPU::indirectX, &CPU::ADC, "ADC", 6}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectX, &CPU::RRA, "RRA", 8},
    {&CPU::zeropage, &CPU::NOP, "NOP", 3}, {&CPU::zeropage, &CPU::ADC, "ADC", 3}, {&CPU::zeropage, &CPU::ROR, "ROR", 5}, {&CPU::zeropage, &CPU::RRA, "RRA", 5},
    {&CPU::implied, &CPU::PLA, "PLA", 4}, {&CPU::immediate, &CPU::ADC, "ADC", 2}, {&CPU::accumulator, &CPU::ROR, "ROR", 2}, {&CPU::immediate, &CPU::ARR, "ARR", 2},
    {&CPU::indirect, &CPU::JMP, "JMP", 5}, {&CPU::absolute, &CPU::ADC, "ADC", 4}, {&CPU::absolute, &CPU::ROR, "ROR", 6}, {&CPU::absolute, &CPU::RRA, "RRA", 6},

    // 0x70 - 0x7F
    {&CPU::relative, &CPU::BVS, "BVS", 2}, {&CPU::indirectY, &CPU::ADC, "ADC", 5}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::RRA, "RRA", 8},
    {&CPU::zeropageX, &CPU::NOP, "NOP", 4}, {&CPU::zeropageX, &CPU::ADC, "ADC", 4}, {&CPU::zeropageX, &CPU::ROR, "ROR", 6}, {&CPU::zeropageX, &CPU::RRA, "RRA", 6},
    {&CPU::implied, &CPU::SEI, "SEI", 2}, {&CPU::absoluteY, &CPU::ADC, "ADC", 4}, {&CPU::implied, &CPU::NOP, "NOP", 2}, {&CPU::absoluteY, &CPU::RRA, "RRA", 7},
    {&CPU::absoluteX, &CPU::NOP, "NOP", 4}, {&CPU::absoluteX, &CPU::ADC, "ADC", 4}, {&CPU::absoluteX, &CPU::ROR, "ROR", 7}, {&CPU::absoluteX, &CPU::RRA, "RRA", 7},

    // 0x80 - 0x8F
    {&CPU::immediate, &CPU::NOP, "NOP", 2}, {&CPU::indirectX, &CPU::STA, "STA", 6}, {&CPU::immediate, &CPU::NOP, "NOP", 2}, {&CPU::indirectX, &CPU::SAX, "SAX", 6},
    {&CPU::zeropage, &CPU::STY, "STY", 3}, {&CPU::zeropage, &CPU::STA, "STA", 3}, {&CPU::zeropage, &CPU::STX, "STX", 3}, {&CPU::zeropage, &CPU::SAX, "SAX", 3},
    {&CPU::implied, &CPU::DEY, "DEY", 2}, {&CPU::immediate, &CPU::NOP, "NOP", 2}, {&CPU::implied, &CPU::TXA, "TXA", 2}, {&CPU::immediate, &CPU::XAA, "XAA", 2},
    {&CPU::absolute, &CPU::STY, "STY", 4}, {&CPU::absolute, &CPU::STA, "STA", 4}, {&CPU::absolute, &CPU::STX, "STX", 4}, {&CPU::absolute, &CPU::SAX, "SAX", 4},

    // 0x90 - 0x9F
    {&CPU::relative, &CPU::BCC, "BCC", 2}, {&CPU::indirectY, &CPU::STA, "STA", 6}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::SHA, "SHA", 6},
    {&CPU::zeropageX, &CPU::STY, "STY", 4}, {&CPU::zeropageX, &CPU::STA, "STA", 4}, {&CPU::zeropageY, &CPU::STX, "STX", 4}, {&CPU::zeropageY, &CPU::SAX, "SAX", 4},
    {&CPU::implied, &CPU::TYA, "TYA", 2}, {&CPU::absoluteY, &CPU::STA, "STA", 5}, {&CPU::implied, &CPU::TXS, "TXS", 2}, {&CPU::absoluteY, &CPU::TAS, "TAS", 5},
    {&CPU::absoluteX, &CPU::SHY, "SHY", 5}, {&CPU::absoluteX, &CPU::STA, "STA", 5}, {&CPU::absoluteY, &CPU::SHX, "SHX", 5}, {&CPU::absoluteY, &CPU::SHA, "SHA", 5},

    // 0xA0 - 0xAF
    {&CPU::immediate, &CPU::LDY, "LDY", 2}, {&CPU::indirectX, &CPU::LDA, "LDA", 6}, {&CPU::immediate, &CPU::LDX, "LDX", 2}, {&CPU::indirectX, &CPU::LAX, "LAX", 6},
    {&CPU::zeropage, &CPU::LDY, "LDY", 3}, {&CPU::zeropage, &CPU::LDA, "LDA", 3}, {&CPU::zeropage, &CPU::LDX, "LDX", 3}, {&CPU::zeropage, &CPU::LAX, "LAX", 3},
    {&CPU::implied, &CPU::TAY, "TAY", 2}, {&CPU::immediate, &CPU::LDA, "LDA", 2}, {&CPU::implied, &CPU::TAX, "TAX", 2}, {&CPU::immediate, &CPU::LXA, "LXA", 2},
    {&CPU::absolute, &CPU::LDY, "LDY", 4}, {&CPU::absolute, &CPU::LDA, "LDA", 4}, {&CPU::absolute, &CPU::LDX, "LDX", 4}, {&CPU::absolute, &CPU::LAX, "LAX", 4},

    // 0xB0 - 0xBF
    {&CPU::relative, &CPU::BCS, "BCS", 2}, {&CPU::indirectY, &CPU::LDA, "LDA", 5}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::LAX, "LAX", 5},
    {&CPU::zeropageX, &CPU::LDY, "LDY", 4}, {&CPU::zeropageX, &CPU::LDA, "LDA", 4}, {&CPU::zeropageY, &CPU::LDX, "LDX", 4}, {&CPU::zeropageY, &CPU::LAX, "LAX", 4},
    {&CPU::implied, &CPU::CLV, "CLV", 2}, {&CPU::absoluteY, &CPU::LDA, "LDA", 4}, {&CPU::implied, &CPU::TSX, "TSX", 2}, {&CPU::absoluteY, &CPU::LAS, "LAS", 4},
    {&CPU::absoluteX, &CPU::LDY, "LDY", 4}, {&CPU::absoluteX, &CPU::LDA, "LDA", 4}, {&CPU::absoluteY, &CPU::LDX, "LDX", 4}, {&CPU::absoluteY, &CPU::LAX, "LAX", 4},

    // 0xC0 - 0xCF
    {&CPU::immediate, &CPU::CPY, "CPY", 2}, {&CPU::indirectX, &CPU::CMP, "CMP", 6}, {&CPU::immediate, &CPU::NOP, "NOP", 2}, {&CPU::indirectX, &CPU::DCP, "DCP", 8},
    {&CPU::zeropage, &CPU::CPY, "CPY", 3}, {&CPU::zeropage, &CPU::CMP, "CMP", 3}, {&CPU::zeropage, &CPU::DEC, "DEC", 5}, {&CPU::zeropage, &CPU::DCP, "DCP", 5},
    {&CPU::implied, &CPU::INY, "INY", 2}, {&CPU::immediate, &CPU::CMP, "CMP", 2}, {&CPU::implied, &CPU::DEX, "DEX", 2}, {&CPU::immediate, &CPU::SBX, "SBX", 2},
    {&CPU::absolute, &CPU::CPY, "CPY", 4}, {&CPU::absolute, &CPU::CMP, "CMP", 4}, {&CPU::absolute, &CPU::DEC, "DEC", 6}, {&CPU::absolute, &CPU::DCP, "DCP", 6},

    // 0xD0 - 0xDF
    {&CPU::relative, &CPU::BNE, "BNE", 2}, {&CPU::indirectY, &CPU::CMP, "CMP", 5}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::DCP, "DCP", 8},
    {&CPU::zeropageX, &CPU::NOP, "NOP", 4}, {&CPU::zeropageX, &CPU::CMP, "CMP", 4}, {&CPU::zeropageX, &CPU::DEC, "DEC", 6}, {&CPU::zeropageX, &CPU::DCP, "DCP", 6},
    {&CPU::implied, &CPU::CLD, "CLD", 2}, {&CPU::absoluteY, &CPU::CMP, "CMP", 4}, {&CPU::implied, &CPU::NOP, "NOP", 2}, {&CPU::absoluteY, &CPU::DCP, "DCP", 7},
    {&CPU::absoluteX, &CPU::NOP, "NOP", 4}, {&CPU::absoluteX, &CPU::CMP, "CMP", 4}, {&CPU::absoluteX, &CPU::DEC, "DEC", 7}, {&CPU::absoluteX, &CPU::DCP, "DCP", 7},

    // 0xE0 - 0xEF
    {&CPU::immediate, &CPU::CPX, "CPX", 2}, {&CPU::indirectX, &CPU::SBC, "SBC", 6}, {&CPU::immediate, &CPU::NOP, "NOP", 2}, {&CPU::indirectX, &CPU::ISC, "ISC", 8},
    {&CPU::zeropage, &CPU::CPX, "CPX", 3}, {&CPU::zeropage, &CPU::SBC, "SBC", 3}, {&CPU::zeropage, &CPU::INC, "INC", 5}, {&CPU::zeropage, &CPU::ISC, "ISC", 5},
    {&CPU::implied, &CPU::INX, "INX", 2}, {&CPU::immediate, &CPU::SBC, "SBC", 2}, {&CPU::implied, &CPU::NOP, "NOP", 2}, {&CPU::immediate, &CPU::SBC, "SBC", 2},
    {&CPU::absolute, &CPU::CPX, "CPX", 4}, {&CPU::absolute, &CPU::SBC, "SBC", 4}, {&CPU::absolute, &CPU::INC, "INC", 6}, {&CPU::absolute, &CPU::ISC, "ISC", 6},

    // 0xF0 - 0xFF
    {&CPU::relative, &CPU::BEQ, "BEQ", 2}, {&CPU::indirectY, &CPU::SBC, "SBC", 5}, {&CPU::implied, &CPU::JAM, "JAM", 0}, {&CPU::indirectY, &CPU::ISC, "ISC", 8},
    {&CPU::zeropageX, &CPU::NOP, "NOP", 4}, {&CPU::zeropageX, &CPU::SBC, "SBC", 4}, {&CPU::zeropageX, &CPU::INC, "INC", 6}, {&CPU::zeropageX, &CPU::ISC, "ISC", 6},
    {&CPU::implied, &CPU::SED, "SED", 2}, {&CPU::absoluteY, &CPU::SBC, "SBC", 4}, {&CPU::implied, &CPU::NOP, "NOP", 2}, {&CPU::absoluteY, &CPU::ISC, "ISC", 7},
    {&CPU::absoluteX, &CPU::NOP, "NOP", 4}, {&CPU::absoluteX, &CPU::SBC, "SBC", 4}, {&CPU::absoluteX, &CPU::INC, "INC", 7}, {&CPU::absoluteX, &CPU::ISC, "ISC", 7}
}
{
    // Initialize registers
    reset();
}

void CPU::setRegisters(CPURegisters registers)
{
    this->registers = registers;
}

CPURegisters CPU::getRegisters()
{
    return registers;
}

void CPU::reset()
{
    registers.A = registers.X = registers.Y = 0;
    registers.carryFlag = registers.zeroFlag = false;
    registers.decimalModeFlag = registers.breakCommandFlag = registers.overflowFlag = false;
    registers.negativeFlag = false;
    if (DEBUG)
    {
        registers.PC = 0xC000; // Start of test rom nestest.nes
        registers.SP = 0xFD;   // Starting point of the stack addressed at 0x0100-0x01FF
    }
    else
    {
        registers.PC = 0x8000;
        registers.SP = 0xFF;
    }
    registers.interruptDisableFlag = true;
    registers.cycles = 0;
}

void CPU::execute()
{
    // Fetch the opcode
    uint8_t opcode = bus.readMemory(registers.PC);
    // dissassemble(opcode, operand);

    currentOpcode = opcodes[opcode];

    if (opcodes[opcode].name == "JAM")
    {
        (this->*currentOpcode.handler)();

        return;
    }

    registers.PC++;

    // Fetch the operand
    (this->*currentOpcode.addressing)();

    // Execute the opcode
    (this->*currentOpcode.handler)();

    // Add the number of cycles to the total number of cycles
    registers.cycles += opcodes[opcode].cycles;
}

DisassembledOpcode CPU::dissassemble(uint8_t opcode)
{
    DisassembledOpcode result;

    result.opcode = opcode;
    result.CYC = opcodes[opcode].cycles;
    result.name = opcodes[opcode].name;

    std::stringstream operandStream;
    std::stringstream registersStream;

    operandStream << std::hex << std::setfill('0');
    registersStream << "A:" << std::setw(2) << static_cast<int>(registers.A) << " X:" << std::setw(2) << static_cast<int>(registers.X) << " Y:" << std::setw(2) << static_cast<int>(registers.Y) << " P:";

    if (opcodes[opcode].addressing == &CPU::immediate)
    {
        result.addressingMode = "immediate";
        operandStream << "#$" << std::setw(2) << static_cast<int>(bus.readMemory(registers.PC + 1));
    }
    else if (opcodes[opcode].addressing == &CPU::implied)
    {
        result.addressingMode = "implied";
    }
    else if (opcodes[opcode].addressing == &CPU::accumulator)
    {
        result.addressingMode = "accumulator";
    }
    else if (opcodes[opcode].addressing == &CPU::zeropage)
    {
        result.addressingMode = "zeropage";
        operandStream << "$" << std::setw(2) << static_cast<int>(bus.readMemory(registers.PC + 1));
    }
    else if (opcodes[opcode].addressing == &CPU::zeropageX)
    {
        result.addressingMode = "zeropageX";
        operandStream << "$" << std::setw(2) << static_cast<int>(bus.readMemory(registers.PC + 1));
    }
    else if (opcodes[opcode].addressing == &CPU::zeropageY)
    {
        result.addressingMode = "zeropageY";
        operandStream << "$" << std::setw(2) << static_cast<int>(bus.readMemory(registers.PC + 1));
    }
    else if (opcodes[opcode].addressing == &CPU::absolute)
    {
        result.addressingMode = "absolute";
        uint16_t address = bus.readMemory(registers.PC + 1) | (bus.readMemory(registers.PC + 2) << 8);
        // cout << " address: " << address << endl;
        operandStream << "$" << std::setw(4) << address;
    }
    else if (opcodes[opcode].addressing == &CPU::absoluteX)
    {
        result.addressingMode = "absoluteX";
        uint16_t address = bus.readMemory(registers.PC + 1) | (bus.readMemory(registers.PC + 2) << 8);
        operandStream << "$" << std::setw(4) << address;
    }
    else if (opcodes[opcode].addressing == &CPU::absoluteY)
    {
        result.addressingMode = "absoluteY";
        uint16_t address = bus.readMemory(registers.PC + 1) | (bus.readMemory(registers.PC + 2) << 8);
        operandStream << "$" << std::setw(4) << address;
    }
    else if (opcodes[opcode].addressing == &CPU::indirectX)
    {
        result.addressingMode = "indirectX";
        operandStream << "$" << std::setw(2) << static_cast<int>(bus.readMemory(registers.PC + 1));
    }
    else if (opcodes[opcode].addressing == &CPU::indirectY)
    {
        result.addressingMode = "indirectY";
        operandStream << "$" << std::setw(2) << static_cast<int>(bus.readMemory(registers.PC + 1));
    }
    else if (opcodes[opcode].addressing == &CPU::relative)
    {
        result.addressingMode = "relative";
        operandStream << "$" << std::setw(2) << static_cast<int>(bus.readMemory(registers.PC + 1));
    }
    else if (opcodes[opcode].addressing == &CPU::indirect)
    {
        result.addressingMode = "indirect";
        uint16_t address = bus.readMemory(registers.PC + 1) | (bus.readMemory(registers.PC + 2) << 8);
        operandStream << "($" << std::setw(4) << address << ")";
    }

    uint8_t flags = 0;
    flags |= registers.carryFlag << 0;
    flags |= registers.zeroFlag << 1;
    flags |= registers.interruptDisableFlag << 2;
    flags |= registers.decimalModeFlag << 3;
    flags |= registers.breakCommandFlag << 4;
    flags |= 1 << 5; // This bit has no use but is always set to 1
    flags |= registers.overflowFlag << 6;
    flags |= registers.negativeFlag << 7;

    registersStream << std::setw(2) << static_cast<int>(flags) << " SP:" << std::setw(2) << static_cast<int>(registers.SP);

    result.A = registers.A;
    result.X = registers.X;
    result.Y = registers.Y;
    result.P = flags;
    result.SP = registers.SP;
    result.operand = operandStream.str();
    // result.registers = registersStream.str();

    return result;
}

uint16_t CPU::fetch()
{
    void (CPU::*addressingMode)() = currentOpcode.addressing;

    // We only need to fetch the operand from memory if the addressing mode is not implied, immediate, or accumulator.
    if (!(addressingMode == &CPU::implied || addressingMode == &CPU::immediate || addressingMode == &CPU::accumulator))
    {
        return bus.readMemory(operand);
    }
    return operand;
}

void CPU::step()
{
    if (!halted)
    {
        execute();
    }
}

void CPU::storeProcessorStatus()
{

    uint8_t flags = 0;
    flags |= registers.carryFlag << 0;
    flags |= registers.zeroFlag << 1;
    flags |= registers.interruptDisableFlag << 2;
    flags |= registers.decimalModeFlag << 3;
    flags |= 1 << 4; // Set the break flag to 1
    flags |= 1 << 5; // This bit has no use but is always set to 1
    flags |= registers.overflowFlag << 6;
    flags |= registers.negativeFlag << 7;

    bus.writeMemory(0x100 + (registers.SP--), flags);
}

void CPU::loadProcessorStatus()
{
    uint8_t flags = bus.readMemory(0x100 + (++registers.SP));

    // Read the registers back into their respective flags.
    registers.carryFlag = (flags >> 0) & 1;
    registers.zeroFlag = (flags >> 1) & 1;
    registers.interruptDisableFlag = (flags >> 2) & 1;
    registers.decimalModeFlag = (flags >> 3) & 1;
    // registers.breakCommandFlag = (flags >> 4) & 1;
    registers.overflowFlag = (flags >> 6) & 1;
    registers.negativeFlag = (flags >> 7) & 1;
}
// NES mos 6502 opcodes

uint8_t CPU::ADC()
{
    uint16_t fetched = fetch();
    uint16_t result = registers.A + fetched + registers.carryFlag;
    registers.carryFlag = result > 0xFF;
    registers.zeroFlag = (result & 0x00FF) == 0;
    registers.negativeFlag = result & 0x80;
    registers.overflowFlag = ((registers.A ^ result) & (fetched ^ result) & 0x80) != 0;

    registers.A = result;

    return 1;
}

uint8_t CPU::AND()
{
    registers.A = registers.A & fetch();
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;
    return 1;
}

uint8_t CPU::ALR()
{
    uint8_t fetched = fetch();

    registers.A &= fetched;

    uint8_t result = registers.A >> 1;

    registers.carryFlag = registers.A & 0x01;
    registers.A = result;
    registers.zeroFlag = (registers.A & 0xff) == 0;
    registers.negativeFlag = 0;

    return 0;
}

uint8_t CPU::ARR()
{
    uint8_t result = registers.A & fetch();
    uint8_t carry = registers.carryFlag;

    registers.carryFlag = result & 0x01;
    result = (result >> 1) | (carry << 7);

    uint8_t bit5 = (result >> 5) & 1;
    uint8_t bit6 = (result >> 6) & 1;

    registers.A = result;

    registers.carryFlag = bit6;
    registers.overflowFlag = bit5 ^ bit6;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::ANC()
{
    uint8_t fetched = fetch();
    uint8_t result = registers.A & fetched;
    registers.carryFlag = result & 0x80;
    registers.zeroFlag = result == 0;
    registers.negativeFlag = result & 0x80;
    registers.A = result;

    return 0;
}

uint8_t CPU::SBX()
{
    uint8_t fetched = fetch();
    uint16_t result = (registers.X & registers.A) - fetched;
    registers.X = result;
    registers.carryFlag = (result <= 0xFF);
    registers.zeroFlag = result == 0;
    registers.negativeFlag = result & 0x80;
    return 0;
}

uint8_t CPU::ASL()
{
    uint16_t fetched = fetch();
    registers.carryFlag = fetched & 0x80;
    fetched <<= 1;
    registers.zeroFlag = (fetched & 0xff) == 0;
    registers.negativeFlag = fetched & 0x80;

    if (currentOpcode.addressing == &CPU::accumulator)
    {
        registers.A = fetched;
    }
    else
    {
        bus.writeMemory(operand, (fetched & 0xFF));
    }
    return 0;
}

uint8_t CPU::BCC()
{
    if (!registers.carryFlag)
    {
        registers.cycles++;

        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::BCS()
{
    if (registers.carryFlag)
    {
        registers.cycles++;

        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::BEQ()
{
    if (registers.zeroFlag)
    {
        registers.cycles++;

        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::BIT()
{
    uint8_t value = bus.readMemory(operand);
    registers.zeroFlag = (value & registers.A) == 0;
    registers.overflowFlag = value & (1 << 6);
    registers.negativeFlag = value & (1 << 7);

    return 0;
}

uint8_t CPU::BMI()
{
    if (registers.negativeFlag)
    {
        registers.cycles++;
        // Check if the branch is on the same page if not increment the number of cycles
        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::BNE()
{
    if (!registers.zeroFlag)
    {
        registers.cycles++;

        // Check if the branch is on the same page if not increment the number of cycles
        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::BPL()
{
    if (!registers.negativeFlag)
    {
        registers.cycles++;

        // Check if the branch is on the same page if not increment the number of cycles
        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::BRK()
{
    registers.PC++;
    registers.breakCommandFlag = true;

    bus.writeMemory(0x100 + (registers.SP--), registers.PC >> 8);
    bus.writeMemory(0x100 + (registers.SP--), registers.PC & 0xFF);
    // Combine the flags into a single 8 byte value for storage and later retrieval
    storeProcessorStatus();
    registers.breakCommandFlag = false;
    registers.interruptDisableFlag = true;
    registers.PC = (bus.readMemory(0xFFFF) << 8) | bus.readMemory(0xFFFE);

    return 0;
}

uint8_t CPU::BVC()
{
    if (!registers.overflowFlag)
    {
        registers.cycles++;
        // Check if the branch is on the same page if not increment the number of cycles
        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::BVS()
{
    if (registers.overflowFlag)
    {
        registers.cycles++;
        // Check if the branch is on the same page if not increment the number of cycles
        if ((operand & 0xFF00) != (registers.PC & 0xFF00))
        {
            registers.cycles++;
        }

        registers.PC = operand;
    }

    return 0;
}

uint8_t CPU::CLC()
{
    registers.carryFlag = false;

    return 0;
}

uint8_t CPU::CLD()
{
    registers.decimalModeFlag = false;

    return 0;
}

uint8_t CPU::CLI()
{
    registers.interruptDisableFlag = false;

    return 0;
}

uint8_t CPU::CLV()
{
    registers.overflowFlag = false;

    return 0;
}

uint8_t CPU::CMP()
{
    uint16_t value = fetch();
    registers.carryFlag = registers.A >= value;
    registers.zeroFlag = registers.A == value;
    registers.negativeFlag = (registers.A - value) & 0x80;

    return 1;
}

uint8_t CPU::CPX()
{
    uint16_t fetched = fetch();
    registers.carryFlag = registers.X >= fetched;
    registers.zeroFlag = registers.X == fetched;
    registers.negativeFlag = (registers.X - fetched) & 0x80;

    return 0;
}

uint8_t CPU::CPY()
{
    uint16_t fetched = fetch();
    registers.carryFlag = registers.Y >= fetched;
    registers.zeroFlag = registers.Y == fetched;
    registers.negativeFlag = (registers.Y - fetched) & 0x80;

    return 0;
}

uint8_t CPU::DEC()
{
    uint8_t fetched = fetch();
    fetched--;
    bus.writeMemory(operand, fetched);
    registers.zeroFlag = fetched == 0;
    registers.negativeFlag = fetched & 0x80;

    return 0;
}

uint8_t CPU::DEX()
{
    registers.X--;
    registers.zeroFlag = registers.X == 0;
    registers.negativeFlag = registers.X & 0x80;

    return 0;
}

uint8_t CPU::DCP()
{
    uint8_t value = fetch();
    value--;
    bus.writeMemory(operand, value);

    uint16_t result = registers.A - value;

    registers.carryFlag = (registers.A >= value);
    registers.zeroFlag = ((result & 0x00FF) == 0);
    registers.negativeFlag = (result & 0x80);

    return 0;
}

uint8_t CPU::DEY()
{
    registers.Y--;
    registers.zeroFlag = registers.Y == 0;
    registers.negativeFlag = registers.Y & 0x80;

    return 0;
}

uint8_t CPU::EOR()
{
    registers.A = registers.A ^ fetch();
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 1;
}

uint8_t CPU::INC()
{
    uint8_t fetched = fetch();
    fetched++;
    bus.writeMemory(operand, fetched);
    registers.zeroFlag = fetched == 0;
    registers.negativeFlag = fetched & 0x80;

    return 0;
}

uint8_t CPU::INX()
{
    registers.X++;
    registers.zeroFlag = registers.X == 0;
    registers.negativeFlag = registers.X & 0x80;

    return 0;
}

uint8_t CPU::ISC()
{
    uint8_t value = fetch();
    value++;
    bus.writeMemory(operand, value);

    uint16_t result = registers.A - value - (1 - registers.carryFlag);

    registers.carryFlag = (result <= 0xFF);
    registers.zeroFlag = ((result & 0x00FF) == 0);
    registers.negativeFlag = (result & 0x80);
    registers.overflowFlag = ((registers.A ^ value) & (registers.A ^ result) & 0x80);

    registers.A = result & 0x00FF;

    return 0;
}

uint8_t CPU::INY()
{
    registers.Y++;
    registers.zeroFlag = registers.Y == 0;
    registers.negativeFlag = registers.Y & 0x80;

    return 0;
}

uint8_t CPU::JAM()
{
    halted = true;
    return 0;
}

uint8_t CPU::JMP()
{
    registers.PC = operand;
    return 0;
}

uint8_t CPU::JSR()
{
    registers.PC--;
    bus.writeMemory(0x0100 + (registers.SP--), (registers.PC >> 8) & 0xFF);
    bus.writeMemory(0x0100 + (registers.SP--), registers.PC & 0xFF);

    registers.PC = operand;

    return 0;
}

uint8_t CPU::LAX()
{
    registers.A = fetch();
    registers.X = registers.A;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 1;
}

uint8_t CPU::LAS()
{
    registers.A = bus.readMemory(operand) & registers.SP;
    registers.X = registers.A;
    registers.SP = registers.A;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::LDA()
{
    registers.A = fetch();
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 1;
}

uint8_t CPU::LDX()
{
    registers.X = fetch();
    registers.zeroFlag = registers.X == 0;
    registers.negativeFlag = registers.X & 0x80;

    return 1;
}

uint8_t CPU::LDY()
{
    registers.Y = fetch();
    registers.zeroFlag = registers.Y == 0;
    registers.negativeFlag = registers.Y & 0x80;

    return 1;
}

uint8_t CPU::LSR()
{
    uint16_t fetched = fetch();
    uint16_t address = fetched;
    registers.carryFlag = fetched & 0x01;
    fetched >>= 1;
    registers.zeroFlag = (fetched & 0xff) == 0;
    registers.negativeFlag = 0;

    // check if LSR is using the accumulator addressing mode
    if (currentOpcode.addressing == &CPU::accumulator)
    {
        registers.A = fetched;
    }
    else
    {
        bus.writeMemory(operand, (fetched & 0xFF));
    }

    return 0;
}

uint8_t CPU::LXA()
{
    registers.A = (registers.A | 0xEE) & fetch();
    registers.X = registers.A;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::NOP()
{
    // This operation does nothing
    if (bus.readMemory(registers.PC - 1) == 0xEA)
    {
        return 1;
    }

    return 0;
}

uint8_t CPU::ORA()
{
    registers.A = registers.A | fetch();
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 1;
}

uint8_t CPU::PHA()
{
    bus.writeMemory(0x100 + (registers.SP--), registers.A);

    return 0;
}

uint8_t CPU::PHP()
{
    // Combine the flags into a single 8 byte value for storage and later retrieval
    storeProcessorStatus();

    return 0;
}

uint8_t CPU::PLA()
{
    registers.A = bus.readMemory(0x100 + (++registers.SP));
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::PLP()
{
    loadProcessorStatus();

    return 0;
}

uint8_t CPU::RLA()
{
    uint8_t value = fetch();
    uint8_t carry = registers.carryFlag;
    registers.carryFlag = value & 0x80;
    value = (value << 1) | carry;
    registers.A &= value;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::RRA()
{
    uint8_t value = fetch();
    uint8_t carry = registers.carryFlag;
    registers.carryFlag = value & 0x01;

    value = (value >> 1) | (carry << 7);
    
    bus.writeMemory(operand, value);

    uint16_t result = registers.A + value + registers.carryFlag;

    registers.carryFlag = (result > 0xFF);
    registers.zeroFlag = ((result & 0xFF) == 0);
    registers.negativeFlag = (result & 0x80) != 0;
    registers.overflowFlag = ((registers.A ^ result) & (value ^ result) & 0x80) != 0;
    registers.A = result & 0xFF;

    return 0;
}

uint8_t CPU::ROL()
{
    uint16_t fetched = fetch();
    uint16_t address = operand;
    uint8_t carry = registers.carryFlag;
    registers.carryFlag = fetched & 0x80;
    fetched = (fetched << 1) | carry; // Store previous carry value into bit 0 of fetched
    registers.zeroFlag = (fetched & 0xff) == 0;
    registers.negativeFlag = fetched & 0x80;

    if (currentOpcode.addressing == &CPU::accumulator)
    {
        registers.A = fetched;
    }
    else
    {
        bus.writeMemory(operand, (fetched & 0xFF));
    }

    return 0;
}

uint8_t CPU::ROR()
{
    uint16_t fetched = fetch();
    uint16_t address = fetched;
    uint8_t carry = registers.carryFlag;
    registers.carryFlag = fetched & 0x01;
    fetched = (fetched >> 1) | (carry << 7); // Store previous carry value into bit 7 of fetched
    registers.zeroFlag = fetched == 0;
    registers.negativeFlag = fetched & 0x80;

    if (currentOpcode.addressing == &CPU::accumulator)
    {
        registers.A = fetched;
    }
    else
    {
        bus.writeMemory(operand, (fetched & 0xFF));
    }

    return 0;
}

uint8_t CPU::RTI()
{
    loadProcessorStatus();
    registers.PC = bus.readMemory(0x100 + (++registers.SP));
    registers.PC |= bus.readMemory(0x100 + (++registers.SP)) << 8;
    return 0;
}

uint8_t CPU::RTS()
{
    uint8_t L = bus.readMemory(0x100 + (++registers.SP));
    uint8_t H = bus.readMemory(0x100 + (++registers.SP));

    registers.PC = (H << 8) | L;
    registers.PC++;

    return 1;
}

uint8_t CPU::SBC()
{
    uint16_t fetched = fetch();
    uint16_t result = registers.A - fetched - !registers.carryFlag;

    registers.carryFlag = result <= 0xFF;
    registers.zeroFlag = (result & 0xff) == 0;
    registers.negativeFlag = result & 0x80;
    registers.overflowFlag = ((registers.A ^ result) & ~(fetched ^ result) & 0x80) != 0;

    registers.A = result;

    return 1;
}

uint8_t CPU::SAX()
{
    uint8_t result = registers.A & registers.X;
    bus.writeMemory(operand, result);
    return 0;
}

uint8_t CPU::SHA()
{
    uint8_t results = registers.A & registers.X & ((operand >> 8) + 1);
    bus.writeMemory(operand, results);

    return 0;
}

uint8_t CPU::SHY()
{
    uint8_t results = registers.Y & ((operand >> 8) + 1);
    bus.writeMemory(operand, results);

    return 0;
}

uint8_t CPU::SHX()
{
    uint8_t results = registers.X & ((operand >> 8) + 1);
    bus.writeMemory(operand, results);

    return 0;
}

uint8_t CPU::SLO()
{
    uint8_t value = fetch();
    registers.carryFlag = value & 0x80;
    value <<= 1;
    registers.A |= value;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::SRE()
{
    uint8_t value = fetch();
    registers.carryFlag = value & 0x01;
    value >>= 1;
    registers.A ^= value;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;
    return 0;
}

uint8_t CPU::SEC()
{
    registers.carryFlag = true;
    return 0;
}

uint8_t CPU::SED()
{
    registers.decimalModeFlag = true;
    return 0;
}

uint8_t CPU::SEI()
{
    registers.interruptDisableFlag = true;
    return 0;
}

uint8_t CPU::STA()
{
    bus.writeMemory(operand, registers.A);

    return 0;
}

uint8_t CPU::STX()
{
    bus.writeMemory(operand, registers.X);

    return 0;
}

uint8_t CPU::STY()
{
    bus.writeMemory(operand, registers.Y);

    return 0;
}

uint8_t CPU::TAX()
{
    registers.X = registers.A;
    registers.zeroFlag = registers.X == 0;
    registers.negativeFlag = registers.X & 0x80;

    return 0;
}

uint8_t CPU::TAY()
{
    registers.Y = registers.A;
    registers.zeroFlag = registers.Y == 0;
    registers.negativeFlag = registers.Y & 0x80;

    return 0;
}

uint8_t CPU::TSX()
{
    registers.X = registers.SP;
    registers.zeroFlag = registers.X == 0;
    registers.negativeFlag = registers.X & 0x80;

    return 0;
}

uint8_t CPU::TAS()
{
    registers.SP = registers.A & registers.X;
    uint8_t results = registers.SP & ((operand >> 8) + 1);
    bus.writeMemory(operand, results);
    return 0;
}

uint8_t CPU::TXA()
{
    registers.A = registers.X;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::TXS()
{
    registers.SP = registers.X;

    return 0;
}

uint8_t CPU::TYA()
{
    registers.A = registers.Y;
    registers.zeroFlag = registers.A == 0;
    registers.negativeFlag = registers.A & 0x80;

    return 0;
}

uint8_t CPU::XAA()
{
    // This opcode is known for it's nondeterministic behavior, as such, it's use should be avoided,
    // however an implemenation is included for completeness.

    uint8_t fetched = fetch();

    // Define a constant value, which could be $00, $FF, $EE, etc.
    // Choosing 0xEE for this implementation to avoid instability.
    uint8_t CONST = 0xEE;
 
    registers.A = (registers.A | CONST) & registers.X & fetched;
    registers.zeroFlag = (registers.A == 0);
    registers.negativeFlag = (registers.A & 0x80) != 0;

    return 0;
}

// 6502 addressing modes
void CPU::accumulator()
{
    operand = registers.A;
}

void CPU::absolute()
{
    uint16_t L = bus.readMemory(registers.PC);
    registers.PC++;
    uint16_t H = bus.readMemory(registers.PC);
    registers.PC++;
    H = (H << 8) | L;
    operand = H;
}

void CPU::absoluteX()
{
    uint16_t L = bus.readMemory(registers.PC);
    registers.PC++;
    uint16_t H = bus.readMemory(registers.PC);
    registers.PC++;


    uint16_t baseAddress = ((H << 8) | L);
    uint16_t effectiveAddress = baseAddress + registers.X;
    
    // Check for page boundary crossing
    if ((effectiveAddress & 0xFF00) != (baseAddress & 0xFF00))
    {
        registers.cycles++;
    }

    operand = effectiveAddress;
}

void CPU::absoluteY()
{
    uint16_t L = bus.readMemory(registers.PC);
    registers.PC++;
    uint16_t H = bus.readMemory(registers.PC);
    registers.PC++;


    uint16_t baseAddress = ((H << 8) | L);
    uint16_t effectiveAddress = baseAddress + registers.Y;
    
    // Check for page boundary crossing
    if ((effectiveAddress & 0xFF00) != (baseAddress & 0xFF00))
    {
        registers.cycles++;
    }

    operand = effectiveAddress;
}

void CPU::immediate()
{
    operand = bus.readMemory(registers.PC++);
}

void CPU::implied()
{
    operand = registers.A;
}

void CPU::indirect()
{
    uint16_t pointerLow = bus.readMemory(registers.PC);
    registers.PC++;
    uint16_t pointerHigh = bus.readMemory(registers.PC);
    registers.PC++;

    uint16_t pointer = (pointerHigh << 8) | pointerLow;

    // Handle page boundary bug
    if (pointerLow == 0x00FF)
    {
        operand = (bus.readMemory(pointer & 0xFF00) << 8) | bus.readMemory(pointer);
    }
    else
    {
        operand = (bus.readMemory(pointer + 1) << 8) | bus.readMemory(pointer);
    }
}

void CPU::indirectX()
{
    // Fetch the byte following the opcode. This is the zero-page address.
    uint8_t zeroPageAddr = bus.readMemory(registers.PC++);

    // Add X to the zero-page address. Wraparound is automatic due to uint8_t overflow.
    uint8_t effectiveZeroPageAddr = zeroPageAddr + registers.X;

    // Fetch the effective address from zero-page memory.
    // Note: Wrapping occurs within the zero-page for the second byte fetch.
    uint16_t L = bus.readMemory(effectiveZeroPageAddr);
    uint16_t H = bus.readMemory((effectiveZeroPageAddr + 1) & 0xFF); // Ensure wraparound within zero-page

    // Construct the effective address.
    operand = (H << 8) | L;
}

void CPU::indirectY()
{

    // Just modified this don't think it does what I want it too yet
    uint16_t zeroPageAddr = bus.readMemory(registers.PC++);

    // Fetch the effective address from zero-page memory.
    uint16_t L = bus.readMemory(zeroPageAddr);
    uint16_t H = bus.readMemory((zeroPageAddr + 1) & 0xFF); // Ensure wraparound within zero-page

    uint16_t indirectAddress = (H << 8) | L;
    uint16_t effectiveAddress = indirectAddress + registers.Y;

    if ((indirectAddress & 0xFF00) != (effectiveAddress & 0xFF00))
    {
        registers.cycles++;
    }
    
    // Construct the effective address.
    operand = effectiveAddress;
}

void CPU::relative()
{
    int8_t relative_address = bus.readMemory(registers.PC);
    registers.PC++;

    // Verify that the address is in the range -128 to 127
    if (relative_address & 0x80)
    {
        relative_address |= 0xFF00;
    }
    operand = registers.PC + relative_address;
}

void CPU::zeropage()
{
    uint8_t address = bus.readMemory(registers.PC);
    registers.PC++;
    operand = address & 0x00FF;
}

void CPU::zeropageX()
{
    uint8_t address = bus.readMemory(registers.PC);
    registers.PC++;
    uint16_t effectiveAddress = address + registers.X;
    
    operand = effectiveAddress & 0x00FF;
}

void CPU::zeropageY()
{
    uint8_t address = bus.readMemory(registers.PC);
    address += registers.Y;
    registers.PC++;
    operand = address & 0x00FF;
}
