#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <string>

class Bus;

struct CPURegisters
{
    // General purpose registers
    uint8_t A; // Accumulator
    uint8_t X; // Index register X
    uint8_t Y; // Index register Y

    // Status flag registers
    bool carryFlag;
    bool zeroFlag;
    bool interruptDisableFlag;
    bool decimalModeFlag; // Don't believe the NES implementation of the mos 6502 supported decimal mode, might remove this.
    bool breakCommandFlag;
    bool overflowFlag;
    bool negativeFlag;

    // Program Counter and Stack Pointer
    uint16_t PC;
    uint8_t SP;
};

struct DisassembledOpcode {
        uint8_t opcode;
        uint8_t A;
        uint8_t X;
        uint8_t Y;
        uint8_t P;
        uint8_t SP;
        uint32_t CYC;
        std::string name;
        std::string addressingMode;
        std::string operand;
};

class CPU
{
public:
    CPU(Bus &bus);
    uint16_t operand;
    void step();
    void reset();
    void setRegisters(CPURegisters registers);
    uint16_t getCycles();
    DisassembledOpcode dissassemble(uint8_t opcode);
    void execute();
    CPURegisters getRegisters();

private:
    CPURegisters registers;
    uint16_t cycles;
    Bus &bus;
    bool halted = false;
    
    struct opcode {
        void (CPU::*addressing)();
        uint8_t (CPU::*handler)();
        std::string name;
        uint8_t cycles;
    };

    

    opcode currentOpcode;
    opcode opcodes[256];

    void storeProcessorStatus();
    void loadProcessorStatus();

    uint16_t fetch();

    // Mos 6502 opcodes, each function has a return value for if it has an additional cycle.
    // which occurs when a page boundary is crossed.
    uint8_t ADC();
    uint8_t AND();
    uint8_t AHX();
    uint8_t ALR();
    uint8_t ASR();
    uint8_t ARR();
    uint8_t ANC();
    uint8_t ASL();
    uint8_t BCC();
    uint8_t BCS();
    uint8_t BEQ();
    uint8_t BIT();
    uint8_t BMI();
    uint8_t BNE();
    uint8_t BPL();
    uint8_t BRK();
    uint8_t BVC();
    uint8_t BVS();
    uint8_t CLC();
    uint8_t CLD();
    uint8_t CLI();
    uint8_t CLV();
    uint8_t CMP();
    uint8_t CPX();
    uint8_t CPY();
    uint8_t DEC();
    uint8_t DEX();
    uint8_t DCP();
    uint8_t DEY();
    uint8_t EOR();
    uint8_t INC();
    uint8_t INX();
    uint8_t ISC();
    uint8_t INY();
    uint8_t JAM();
    uint8_t JMP();
    uint8_t JSR();
    uint8_t LAX();
    uint8_t LAS();
    uint8_t LDA();
    uint8_t LDX();
    uint8_t LDY();
    uint8_t LSR();
    uint8_t LXA();
    uint8_t NOP();
    uint8_t ORA();
    uint8_t PHA();
    uint8_t PHP();
    uint8_t PLA();
    uint8_t PLP();
    uint8_t RLA();
    uint8_t RRA();
    uint8_t ROL();
    uint8_t ROR();
    uint8_t RTI();
    uint8_t RTS();
    uint8_t SBC();
    uint8_t SBX();
    uint8_t SAX();
    uint8_t SHA();
    uint8_t SHY();
    uint8_t SHX();
    uint8_t SLO();
    uint8_t SRE();
    uint8_t SEC();
    uint8_t SED();
    uint8_t SEI();
    uint8_t STA();
    uint8_t STX();
    uint8_t STY();
    uint8_t TAX();
    uint8_t TAY();
    uint8_t TSX();
    uint8_t TAS();
    uint8_t TXA();
    uint8_t TXS();
    uint8_t TYA();
    uint8_t XAA();
    uint8_t ZZZ();

    // Functions to handle the different addressing modes of the mos 6502.
    void accumulator();
    void absolute();
    void absoluteX();
    void absoluteY();
    void immediate();
    void implied();
    void indirect();
    void indirectX();
    void indirectY();
    void relative();
    void zeropage();
    void zeropageX();
    void zeropageY();
};

#endif
