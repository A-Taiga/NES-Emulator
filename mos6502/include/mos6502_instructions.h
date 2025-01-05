#ifndef MOS6502_INSTRUCTIONS
#define MOS6502_INSTRUCTIONS

#include <array>

namespace _6502
{

    enum class Inst
    {
        BRK, ORA, ASL, PHP, BPL,
        CLC, JSR, AND, BIT, ROL, 
        PLP, BMI, SEC, RTI, EOR,
        LSR, PHA, JMP, BVC, CLI,
        RTS, PLA, ADC, ROR, BVS,
        SEI, STA, STY, STX, DEY,
        TXA, BCC, TYA, TXS, LDY, 
        LDA, LDX, TAY, TAX, BCS, 
        CLV, TSX, CPY, CMP, DEC, 
        INY, DEX, BNE, CLD, CPX, 
        SBC, INC, INX, NOP, BEQ, 
        SED, XXX,
    };

    enum class Mode
    {
        ACC,ABS,ABX,ABY,IMM,IMP,IND,XIZ,YIZ,REL,ZPG,ZPX,ZPY,
    };

    struct Instruction
    {
        Inst instruction;
        Mode mode;

        const char* mnemonic;
        const char* instruction_info;
        const char* mode_info;

        constexpr Instruction (const Inst _instruction = Inst::XXX, const Mode _mode = Mode::IMP)
        : instruction {_instruction}
        , mode {_mode}
        {
            switch (_instruction)
            {
                case Inst::BRK: mnemonic = "BRK"; break; case Inst::ORA: mnemonic = "ORA"; break; case Inst::ASL: mnemonic = "ASL"; break; case Inst::PHP: mnemonic = "PHP"; break; case Inst::BPL: mnemonic = "BPL"; break;
                case Inst::CLC: mnemonic = "CLC"; break; case Inst::JSR: mnemonic = "JSR"; break; case Inst::AND: mnemonic = "AND"; break; case Inst::BIT: mnemonic = "BIT"; break; case Inst::ROL: mnemonic = "ROL"; break; 
                case Inst::PLP: mnemonic = "PLP"; break; case Inst::BMI: mnemonic = "BMI"; break; case Inst::SEC: mnemonic = "SEC"; break; case Inst::RTI: mnemonic = "RTI"; break; case Inst::EOR: mnemonic = "EOR"; break;
                case Inst::LSR: mnemonic = "LSR"; break; case Inst::PHA: mnemonic = "PHA"; break; case Inst::JMP: mnemonic = "JMP"; break; case Inst::BVC: mnemonic = "BVC"; break; case Inst::CLI: mnemonic = "CLI"; break;
                case Inst::RTS: mnemonic = "RTS"; break; case Inst::PLA: mnemonic = "PLA"; break; case Inst::ADC: mnemonic = "ADC"; break; case Inst::ROR: mnemonic = "ROR"; break; case Inst::BVS: mnemonic = "BVS"; break;
                case Inst::SEI: mnemonic = "SEI"; break; case Inst::STA: mnemonic = "STA"; break; case Inst::STY: mnemonic = "STY"; break; case Inst::STX: mnemonic = "STX"; break; case Inst::DEY: mnemonic = "DEY"; break;
                case Inst::TXA: mnemonic = "TXA"; break; case Inst::BCC: mnemonic = "BCC"; break; case Inst::TYA: mnemonic = "TYA"; break; case Inst::TXS: mnemonic = "TXS"; break; case Inst::LDY: mnemonic = "LDY"; break; 
                case Inst::LDA: mnemonic = "LDA"; break; case Inst::LDX: mnemonic = "LDX"; break; case Inst::TAY: mnemonic = "TAY"; break; case Inst::TAX: mnemonic = "TAX"; break; case Inst::BCS: mnemonic = "BCS"; break; 
                case Inst::CLV: mnemonic = "CLV"; break; case Inst::TSX: mnemonic = "TSX"; break; case Inst::CPY: mnemonic = "CPY"; break; case Inst::CMP: mnemonic = "CMP"; break; case Inst::DEC: mnemonic = "DEC"; break; 
                case Inst::INY: mnemonic = "INY"; break; case Inst::DEX: mnemonic = "DEX"; break; case Inst::BNE: mnemonic = "BNE"; break; case Inst::CLD: mnemonic = "CLD"; break; case Inst::CPX: mnemonic = "CPX"; break; 
                case Inst::SBC: mnemonic = "SBC"; break; case Inst::INC: mnemonic = "INC"; break; case Inst::INX: mnemonic = "INX"; break; case Inst::NOP: mnemonic = "NOP"; break; case Inst::BEQ: mnemonic = "BEQ"; break; 
                case Inst::SED: mnemonic = "SED"; break; case Inst::XXX: mnemonic = "XXX"; break;
            }

            switch (_instruction)
            {
                case Inst::ADC: instruction_info = "Add with carry"; break;
                case Inst::AND: instruction_info = "And (with accumulator)"; break;
                case Inst::ASL: instruction_info = "Arithmetic shift left"; break;
                case Inst::BCC: instruction_info = "Branch on carry clear"; break;
                case Inst::BCS: instruction_info = "Branch on carry set"; break;
                case Inst::BEQ: instruction_info = "Branch on equal (zero set)"; break;
                case Inst::BIT: instruction_info = "Bit test"; break;
                case Inst::BMI: instruction_info = "Branch on minus (negative set)"; break;
                case Inst::BNE: instruction_info = "Branch on not equal (zero clear)"; break;
                case Inst::BPL: instruction_info = "Branch on plus (negative clear)"; break;
                case Inst::BRK: instruction_info = "Break / interrupt"; break;
                case Inst::BVC: instruction_info = "Branch on overflow clear"; break;
                case Inst::BVS: instruction_info = "Branch on overflow set"; break;
                case Inst::CLC: instruction_info = "Clear carry"; break;
                case Inst::CLD: instruction_info = "Clear decimal"; break;
                case Inst::CLI: instruction_info = "Clear interrupt disable"; break;
                case Inst::CLV: instruction_info = "Clear overflow"; break;
                case Inst::CMP: instruction_info = "Compare (with accumulator)"; break;
                case Inst::CPX: instruction_info = "Compare with X"; break;
                case Inst::CPY: instruction_info = "Compare with Y"; break;
                case Inst::DEC: instruction_info = "Decrement"; break;
                case Inst::DEX: instruction_info = "Decrement X"; break;
                case Inst::DEY: instruction_info = "Decrement Y"; break;
                case Inst::EOR: instruction_info = "Exclusive or (with accumulator)"; break;
                case Inst::INC: instruction_info = "Increment"; break;
                case Inst::INX: instruction_info = "Increment X"; break;
                case Inst::INY: instruction_info = "Increment Y"; break;
                case Inst::JMP: instruction_info = "Jump"; break;
                case Inst::JSR: instruction_info = "Jump subroutine"; break;
                case Inst::LDA: instruction_info = "Load accumulator"; break;
                case Inst::LDX: instruction_info = "Load X"; break;
                case Inst::LDY: instruction_info = "Load Y"; break;
                case Inst::LSR: instruction_info = "Logical shift right"; break;
                case Inst::NOP: instruction_info = "No operation"; break;
                case Inst::ORA: instruction_info = "Or with accumulator"; break;
                case Inst::PHA: instruction_info = "Push accumulator"; break;
                case Inst::PHP: instruction_info = "Push processor status (SR)"; break;
                case Inst::PLA: instruction_info = "Pull accumulator"; break;
                case Inst::PLP: instruction_info = "Pull processor status (SR)"; break;
                case Inst::ROL: instruction_info = "Rotate left"; break;
                case Inst::ROR: instruction_info = "Rotate right"; break;
                case Inst::RTI: instruction_info = "Return from interrupt"; break;
                case Inst::RTS: instruction_info = "Return from subroutine"; break;
                case Inst::SBC: instruction_info = "Subtract with carry"; break;
                case Inst::SEC: instruction_info = "Set carry"; break;
                case Inst::SED: instruction_info = "Set decimal"; break;
                case Inst::SEI: instruction_info = "Set interrupt disable"; break;
                case Inst::STA: instruction_info = "Store accumulator"; break;
                case Inst::STX: instruction_info = "Store X"; break;
                case Inst::STY: instruction_info = "Store Y"; break;
                case Inst::TAX: instruction_info = "Transfer accumulator to X"; break;
                case Inst::TAY: instruction_info = "Transfer accumulator to Y"; break;
                case Inst::TSX: instruction_info = "Transfer stack pointer to X"; break;
                case Inst::TXA: instruction_info = "Transfer X to accumulator"; break;
                case Inst::TXS: instruction_info = "Transfer X to stack pointer"; break;
                case Inst::TYA: instruction_info = "Transfer Y to accumulator"; break;
                case Inst::XXX: instruction_info = "Illegal opcode"; break;
            }

            switch (mode)
            {
                case Mode::ACC: mode_info = "accumulator"; break;
                case Mode::ABS: mode_info = "absolute"; break;
                case Mode::ABX: mode_info = "absoulte X-indexed"; break;
                case Mode::ABY: mode_info = "absolute Y-indexed"; break;
                case Mode::IMM: mode_info = "immediate"; break;
                case Mode::IMP: mode_info = "implied"; break;
                case Mode::IND: mode_info = "indirect"; break;
                case Mode::XIZ: mode_info = "X-indexed indirect zeropage address"; break;
                case Mode::YIZ: mode_info = "Y-indexed indirect zeropage address"; break;
                case Mode::REL: mode_info = "relative"; break;
                case Mode::ZPG: mode_info = "zeropage"; break;
                case Mode::ZPX: mode_info = "zeropage X-indexed"; break;
                case Mode::ZPY: mode_info = "zeropage Y-indexed"; break;
            }
        }
    };
}


#endif