#include <vector> 

#include "Cpu.h"
#include "Bus.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_module) {

    register_vector<uint8_t>("VectorUint8");
    register_vector<int>("VectorInt");

    // Bind CPURegisters struct
    value_object<CPURegisters>("CPURegisters")
        .field("A", &CPURegisters::A)
        .field("X", &CPURegisters::X)
        .field("Y", &CPURegisters::Y)
        .field("carryFlag", &CPURegisters::carryFlag)
        .field("zeroFlag", &CPURegisters::zeroFlag)
        .field("interruptDisableFlag", &CPURegisters::interruptDisableFlag)
        .field("decimalModeFlag", &CPURegisters::decimalModeFlag)
        .field("breakCommandFlag", &CPURegisters::breakCommandFlag)
        .field("overflowFlag", &CPURegisters::overflowFlag)
        .field("negativeFlag", &CPURegisters::negativeFlag)
        .field("PC", &CPURegisters::PC)
        .field("SP", &CPURegisters::SP);
    
    // Bind DisassembledOpcode struct
    value_object<DisassembledOpcode>("DisassembledOpcode")
        .field("opcode", &DisassembledOpcode::opcode)
        .field("A", &DisassembledOpcode::A)
        .field("X", &DisassembledOpcode::X)
        .field("Y", &DisassembledOpcode::Y)
        .field("P", &DisassembledOpcode::P)
        .field("SP", &DisassembledOpcode::SP)
        .field("CYC", &DisassembledOpcode::CYC)
        .field("name", &DisassembledOpcode::name)
        .field("addressingMode", &DisassembledOpcode::addressingMode)
        .field("operand", &DisassembledOpcode::operand);
        
    // Bind CPU class
    class_<CPU>("CPU")
        .constructor<Bus&>()
        .function("step", &CPU::step)
        .function("reset", &CPU::reset)
        .function("setRegisters", &CPU::setRegisters)
        .function("getRegisters", &CPU::getRegisters)
        .function("getCycles", &CPU::getCycles)
        .function("dissassemble", &CPU::dissassemble);
    
    // Bind Bus class if needed
    class_<Bus>("Bus")
        .constructor<>()
        .function("readMemory", &Bus::readMemory)
        .function("writeMemory", &Bus::writeMemory)
        .function("clearMemory", &Bus::clearMemory)
        .function("loadProgram", &Bus::loadProgram)
        .function("run", &Bus::run);
}
#endif
