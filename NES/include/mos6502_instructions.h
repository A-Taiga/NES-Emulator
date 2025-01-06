#ifndef MOS6502_INSTRUCTIONS
#define MOS6502_INSTRUCTIONS

namespace CPU 
{
    namespace _6502
    {

        enum class Opcode
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

        // for debugger to show info
        struct Instruction
        {
            Opcode instruction;
            Mode mode;

            const char* mnemonic;
            const char* instruction_info;
            const char* mode_info;

            constexpr Instruction (const Opcode _instruction = Opcode::XXX, const Mode _mode = Mode::IMP)
            : instruction {_instruction}
            , mode {_mode}
            {
                switch (_instruction)
                {
                    case Opcode::BRK: mnemonic = "BRK"; break; case Opcode::ORA: mnemonic = "ORA"; break; case Opcode::ASL: mnemonic = "ASL"; break; case Opcode::PHP: mnemonic = "PHP"; break; case Opcode::BPL: mnemonic = "BPL"; break;
                    case Opcode::CLC: mnemonic = "CLC"; break; case Opcode::JSR: mnemonic = "JSR"; break; case Opcode::AND: mnemonic = "AND"; break; case Opcode::BIT: mnemonic = "BIT"; break; case Opcode::ROL: mnemonic = "ROL"; break; 
                    case Opcode::PLP: mnemonic = "PLP"; break; case Opcode::BMI: mnemonic = "BMI"; break; case Opcode::SEC: mnemonic = "SEC"; break; case Opcode::RTI: mnemonic = "RTI"; break; case Opcode::EOR: mnemonic = "EOR"; break;
                    case Opcode::LSR: mnemonic = "LSR"; break; case Opcode::PHA: mnemonic = "PHA"; break; case Opcode::JMP: mnemonic = "JMP"; break; case Opcode::BVC: mnemonic = "BVC"; break; case Opcode::CLI: mnemonic = "CLI"; break;
                    case Opcode::RTS: mnemonic = "RTS"; break; case Opcode::PLA: mnemonic = "PLA"; break; case Opcode::ADC: mnemonic = "ADC"; break; case Opcode::ROR: mnemonic = "ROR"; break; case Opcode::BVS: mnemonic = "BVS"; break;
                    case Opcode::SEI: mnemonic = "SEI"; break; case Opcode::STA: mnemonic = "STA"; break; case Opcode::STY: mnemonic = "STY"; break; case Opcode::STX: mnemonic = "STX"; break; case Opcode::DEY: mnemonic = "DEY"; break;
                    case Opcode::TXA: mnemonic = "TXA"; break; case Opcode::BCC: mnemonic = "BCC"; break; case Opcode::TYA: mnemonic = "TYA"; break; case Opcode::TXS: mnemonic = "TXS"; break; case Opcode::LDY: mnemonic = "LDY"; break; 
                    case Opcode::LDA: mnemonic = "LDA"; break; case Opcode::LDX: mnemonic = "LDX"; break; case Opcode::TAY: mnemonic = "TAY"; break; case Opcode::TAX: mnemonic = "TAX"; break; case Opcode::BCS: mnemonic = "BCS"; break; 
                    case Opcode::CLV: mnemonic = "CLV"; break; case Opcode::TSX: mnemonic = "TSX"; break; case Opcode::CPY: mnemonic = "CPY"; break; case Opcode::CMP: mnemonic = "CMP"; break; case Opcode::DEC: mnemonic = "DEC"; break; 
                    case Opcode::INY: mnemonic = "INY"; break; case Opcode::DEX: mnemonic = "DEX"; break; case Opcode::BNE: mnemonic = "BNE"; break; case Opcode::CLD: mnemonic = "CLD"; break; case Opcode::CPX: mnemonic = "CPX"; break; 
                    case Opcode::SBC: mnemonic = "SBC"; break; case Opcode::INC: mnemonic = "INC"; break; case Opcode::INX: mnemonic = "INX"; break; case Opcode::NOP: mnemonic = "NOP"; break; case Opcode::BEQ: mnemonic = "BEQ"; break; 
                    case Opcode::SED: mnemonic = "SED"; break; case Opcode::XXX: mnemonic = "XXX"; break;
                }

                switch (_instruction)
                {
                    case Opcode::ADC: instruction_info = "Add with carry"; break;
                    case Opcode::AND: instruction_info = "And (with accumulator)"; break;
                    case Opcode::ASL: instruction_info = "Arithmetic shift left"; break;
                    case Opcode::BCC: instruction_info = "Branch on carry clear"; break;
                    case Opcode::BCS: instruction_info = "Branch on carry set"; break;
                    case Opcode::BEQ: instruction_info = "Branch on equal (zero set)"; break;
                    case Opcode::BIT: instruction_info = "Bit test"; break;
                    case Opcode::BMI: instruction_info = "Branch on minus (negative set)"; break;
                    case Opcode::BNE: instruction_info = "Branch on not equal (zero clear)"; break;
                    case Opcode::BPL: instruction_info = "Branch on plus (negative clear)"; break;
                    case Opcode::BRK: instruction_info = "Break / interrupt"; break;
                    case Opcode::BVC: instruction_info = "Branch on overflow clear"; break;
                    case Opcode::BVS: instruction_info = "Branch on overflow set"; break;
                    case Opcode::CLC: instruction_info = "Clear carry"; break;
                    case Opcode::CLD: instruction_info = "Clear decimal"; break;
                    case Opcode::CLI: instruction_info = "Clear interrupt disable"; break;
                    case Opcode::CLV: instruction_info = "Clear overflow"; break;
                    case Opcode::CMP: instruction_info = "Compare (with accumulator)"; break;
                    case Opcode::CPX: instruction_info = "Compare with X"; break;
                    case Opcode::CPY: instruction_info = "Compare with Y"; break;
                    case Opcode::DEC: instruction_info = "Decrement"; break;
                    case Opcode::DEX: instruction_info = "Decrement X"; break;
                    case Opcode::DEY: instruction_info = "Decrement Y"; break;
                    case Opcode::EOR: instruction_info = "Exclusive or (with accumulator)"; break;
                    case Opcode::INC: instruction_info = "Increment"; break;
                    case Opcode::INX: instruction_info = "Increment X"; break;
                    case Opcode::INY: instruction_info = "Increment Y"; break;
                    case Opcode::JMP: instruction_info = "Jump"; break;
                    case Opcode::JSR: instruction_info = "Jump subroutine"; break;
                    case Opcode::LDA: instruction_info = "Load accumulator"; break;
                    case Opcode::LDX: instruction_info = "Load X"; break;
                    case Opcode::LDY: instruction_info = "Load Y"; break;
                    case Opcode::LSR: instruction_info = "Logical shift right"; break;
                    case Opcode::NOP: instruction_info = "No operation"; break;
                    case Opcode::ORA: instruction_info = "Or with accumulator"; break;
                    case Opcode::PHA: instruction_info = "Push accumulator"; break;
                    case Opcode::PHP: instruction_info = "Push processor status (SR)"; break;
                    case Opcode::PLA: instruction_info = "Pull accumulator"; break;
                    case Opcode::PLP: instruction_info = "Pull processor status (SR)"; break;
                    case Opcode::ROL: instruction_info = "Rotate left"; break;
                    case Opcode::ROR: instruction_info = "Rotate right"; break;
                    case Opcode::RTI: instruction_info = "Return from interrupt"; break;
                    case Opcode::RTS: instruction_info = "Return from subroutine"; break;
                    case Opcode::SBC: instruction_info = "Subtract with carry"; break;
                    case Opcode::SEC: instruction_info = "Set carry"; break;
                    case Opcode::SED: instruction_info = "Set decimal"; break;
                    case Opcode::SEI: instruction_info = "Set interrupt disable"; break;
                    case Opcode::STA: instruction_info = "Store accumulator"; break;
                    case Opcode::STX: instruction_info = "Store X"; break;
                    case Opcode::STY: instruction_info = "Store Y"; break;
                    case Opcode::TAX: instruction_info = "Transfer accumulator to X"; break;
                    case Opcode::TAY: instruction_info = "Transfer accumulator to Y"; break;
                    case Opcode::TSX: instruction_info = "Transfer stack pointer to X"; break;
                    case Opcode::TXA: instruction_info = "Transfer X to accumulator"; break;
                    case Opcode::TXS: instruction_info = "Transfer X to stack pointer"; break;
                    case Opcode::TYA: instruction_info = "Transfer Y to accumulator"; break;
                    case Opcode::XXX: instruction_info = "Illegal opcode"; break;
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
}

#endif