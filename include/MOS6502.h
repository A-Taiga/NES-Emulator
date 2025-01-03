#ifndef MOS6502_H
#define MOS6502_H

#include "utility.h"
#include <string>
#include <array>
#include <functional>


/*

https://www.nesdev.org/wiki/Instruction_reference

data is stored in little-endien

PC	program counter	(16 bit)
AC	accumulator	(8 bit)
X	X register	(8 bit)
Y	Y register	(8 bit)
SR	status register [NV-BDIZC]	(8 bit)
SP	stack pointer	(8 bit)


*/

namespace CPU
{
    class MOS6502
    {

        enum class Flag: byte
        {
            N = 1 << 7, // negative
            V = 1 << 6, // overflow
            _ = 1 << 5, // ignored / unused
            B = 1 << 4, // break
            D = 1 << 3, // decimal
            I = 1 << 2, // interrupt (IRQ dissabl)
            Z = 1 << 1, // zero
            C = 1 << 0, // carry
        };

        struct Opcode
        {
            std::string mnemonic;
            void (MOS6502::*opcode)(void);
            int  (MOS6502::*mode)(void);
            int cycles;
        };

    public:

        using write_cb = std::function <void(const word,const byte)>;
        using read_cb = std::function <byte(const word)>;

        MOS6502 (read_cb, write_cb);

        void print_instruction_set (void) const;

        
        std::string get_opcode (const byte instruction) const;


        void decompile (const word mem_size);


    private:

        static constexpr u16 stk_begin = 0x0100;


        /* READ WRITE CALLBACKS */
        write_cb write;
        read_cb  read;


        /* REGISTERS */
        word PC;    // program counter
        byte AC;    // accumulator
        byte X;     // x register
        byte Y;     // y register
        byte SR;    // status register
        byte SP;    // stack pointer
        
        // current instruction info
        struct
        {
            Opcode* ins;
            word address;
            byte data;
            int cycles;
        } current;

        void set_flag (const Flag, const bool);
        void stack_push (const byte val);
        u8 stack_pop (void);

        /* OPCODES */
        void BRK (void); void ORA (void); void ASL (void); void PHP (void); void BPL (void);
        void CLC (void); void JSR (void); void AND (void); void BIT (void); void ROL (void); 
        void PLP (void); void BMI (void); void SEC (void); void RTI (void); void EOR (void);
        void LSR (void); void PHA (void); void JMP (void); void BVC (void); void CLI (void);
        void RTS (void); void PLA (void); void ADC (void); void ROR (void); void BVS (void);
        void SEI (void); void STA (void); void STY (void); void STX (void); void DEY (void);
        void TXA (void); void BCC (void); void TYA (void); void TXS (void); void LDY (void); 
        void LDA (void); void LDX (void); void TAY (void); void TAX (void); void BCS (void); 
        void CLV (void); void TSX (void); void CPY (void); void CMP (void); void DEC (void); 
        void INY (void); void DEX (void); void BNE (void); void CLD (void); void CPX (void); 
        void SBC (void); void INC (void); void INX (void); void NOP (void); void BEQ (void); 
        void SED (void); void XXX (void); // XXX = illegal

        /* ADDRESSING MODES */
        int ACC (void); // accumulator 
        int ABS (void); // absolute
        int ABX (void); // absoulte X-indexed
        int ABY (void); // absolute Y-indexed
        int IMM (void); // immediate
        int IMP (void); // implied
        int IND (void); // indirect
        int XIZ (void); // X-indexed indirect zeropage address
        int YIZ (void); // Y-indexed indirect zeropage address
        int REL (void); // relative
        int ZPG (void); // zeropage
        int ZPX (void); // zeropage X-indexed
        int ZPY (void); // zeropage Y-indexed

        /* LOOKUP TABLE */
        using _ = MOS6502;
        static constexpr std::array<Opcode, 256> instruction_set
        {{
            {"BRK", &_::BRK, &_::IMP, 7}, {"ORA", &_::ORA, &_::XIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"ORA", &_::ORA, &_::ZPG, 3}, {"ASL", &_::ASL, &_::ZPG, 5}, {"???", &_::XXX, &_::IMM, 0}, {"PHP", &_::PHP, &_::IMP, 3}, {"ORA", &_::ORA, &_::IMM, 2}, {"ASL", &_::ASL, &_::ACC, 2}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"ORA", &_::ORA, &_::ABS, 4}, {"ASL", &_::ASL, &_::ABS, 6}, {"???", &_::XXX, &_::IMM, 0}, 
            {"BPL", &_::BPL, &_::REL, 2}, {"ORA", &_::ORA, &_::YIZ, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"ORA", &_::ORA, &_::ZPX, 4}, {"ASL", &_::ASL, &_::ZPX, 6}, {"???", &_::XXX, &_::IMM, 0}, {"CLC", &_::CLC, &_::IMP, 2}, {"ORA", &_::ORA, &_::ABY, 4}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"ORA", &_::ORA, &_::ABX, 4}, {"ASL", &_::ASL, &_::ABX, 7}, {"???", &_::XXX, &_::IMM, 0}, 
            {"JSR", &_::JSR, &_::ABS, 6}, {"AND", &_::AND, &_::XIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"BIT", &_::BIT, &_::ZPG, 3}, {"AND", &_::AND, &_::ZPG, 3}, {"ROL", &_::ROL, &_::ZPG, 5}, {"???", &_::XXX, &_::IMM, 0}, {"PLP", &_::PLP, &_::IMP, 4}, {"AND", &_::AND, &_::IMM, 2}, {"ROL", &_::ROL, &_::ACC, 2}, {"???", &_::XXX, &_::IMM, 0}, {"BIT", &_::BIT, &_::ABS, 4}, {"AND", &_::AND, &_::ABS, 4}, {"ROL", &_::ROL, &_::ABS, 6}, {"???", &_::XXX, &_::IMM, 0}, 
            {"BMI", &_::BMI, &_::REL, 2}, {"AND", &_::AND, &_::YIZ, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"AND", &_::AND, &_::ZPX, 4}, {"ROL", &_::ROL, &_::ZPX, 6}, {"???", &_::XXX, &_::IMM, 0}, {"SEC", &_::SEC, &_::IMP, 2}, {"AND", &_::AND, &_::ABY, 4}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"AND", &_::AND, &_::ABX, 4}, {"ROL", &_::ROL, &_::ABX, 7}, {"???", &_::XXX, &_::IMM, 0}, 
            {"RTI", &_::RTI, &_::IMP, 6}, {"EOR", &_::EOR, &_::XIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"EOR", &_::EOR, &_::ZPG, 3}, {"LSR", &_::LSR, &_::ZPG, 5}, {"???", &_::XXX, &_::IMM, 0}, {"PHA", &_::PHA, &_::IMP, 3}, {"EOR", &_::EOR, &_::IMM, 2}, {"LSR", &_::LSR, &_::ACC, 2}, {"???", &_::XXX, &_::IMM, 0}, {"JMP", &_::JMP, &_::ABS, 3}, {"EOR", &_::EOR, &_::ABS, 4}, {"LSR", &_::LSR, &_::ABS, 6}, {"???", &_::XXX, &_::IMM, 0}, 
            {"BVC", &_::BVC, &_::REL, 2}, {"EOR", &_::EOR, &_::YIZ, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"EOR", &_::EOR, &_::ZPX, 4}, {"LSR", &_::LSR, &_::ZPX, 6}, {"???", &_::XXX, &_::IMM, 0}, {"CLI", &_::CLI, &_::IMP, 2}, {"EOR", &_::EOR, &_::ABY, 4}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"EOR", &_::EOR, &_::ABX, 4}, {"LSR", &_::LSR, &_::ABX, 7}, {"???", &_::XXX, &_::IMM, 0}, 
            {"RTS", &_::RTS, &_::IMP, 6}, {"ADC", &_::ADC, &_::XIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"ADC", &_::ADC, &_::ZPG, 3}, {"ROR", &_::ROR, &_::ZPG, 5}, {"???", &_::XXX, &_::IMM, 0}, {"PLA", &_::PLA, &_::IMP, 4}, {"ADC", &_::ADC, &_::IMM, 2}, {"ROR", &_::ROR, &_::ACC, 2}, {"???", &_::XXX, &_::IMM, 0}, {"JMP", &_::JMP, &_::IND, 5}, {"ADC", &_::ADC, &_::ABS, 4}, {"ROR", &_::ROR, &_::ABS, 6}, {"???", &_::XXX, &_::IMM, 0}, 
            {"BVS", &_::BVS, &_::REL, 2}, {"ADC", &_::ADC, &_::YIZ, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"ADC", &_::ADC, &_::ZPX, 4}, {"ROR", &_::ROR, &_::ZPX, 6}, {"???", &_::XXX, &_::IMM, 0}, {"SEI", &_::SEI, &_::IMP, 2}, {"ADC", &_::ADC, &_::ABY, 4}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"ADC", &_::ADC, &_::ABX, 4}, {"ROR", &_::ROR, &_::ABX, 7}, {"???", &_::XXX, &_::IMM, 0}, 
            {"???", &_::XXX, &_::IMP, 0}, {"STA", &_::STA, &_::XIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"STY", &_::STY, &_::ZPG, 3}, {"STA", &_::STA, &_::ZPG, 3}, {"STX", &_::STX, &_::ZPG, 3}, {"???", &_::XXX, &_::IMM, 0}, {"DEY", &_::DEY, &_::IMP, 2}, {"???", &_::XXX, &_::IMM, 0}, {"TXA", &_::TXA, &_::IMP, 2}, {"???", &_::XXX, &_::IMM, 0}, {"STY", &_::STY, &_::ABS, 4}, {"STA", &_::STA, &_::ABS, 4}, {"STX", &_::STX, &_::ABS, 4}, {"???", &_::XXX, &_::IMM, 0}, 
            {"BCC", &_::BCC, &_::REL, 2}, {"STA", &_::STA, &_::YIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"STY", &_::STY, &_::ZPX, 4}, {"STA", &_::STA, &_::ZPX, 4}, {"STX", &_::STX, &_::ZPY, 4}, {"???", &_::XXX, &_::IMM, 0}, {"TYA", &_::TYA, &_::IMP, 2}, {"STA", &_::STA, &_::ABY, 5}, {"TXS", &_::TXS, &_::IMP, 2}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"STA", &_::STA, &_::ABX, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, 
            {"LDY", &_::LDY, &_::IMM, 2}, {"LDA", &_::LDA, &_::XIZ, 6}, {"LDX", &_::LDX, &_::IMP, 2}, {"???", &_::XXX, &_::IMP, 0}, {"LDY", &_::LDY, &_::ZPG, 3}, {"LDA", &_::LDA, &_::ZPG, 3}, {"LDX", &_::LDX, &_::ZPG, 3}, {"???", &_::XXX, &_::IMM, 0}, {"TAY", &_::TAY, &_::IMP, 2}, {"LDA", &_::LDA, &_::IMM, 2}, {"TAX", &_::TAX, &_::IMP, 2}, {"???", &_::XXX, &_::IMM, 0}, {"LDY", &_::LDY, &_::ABS, 4}, {"LDA", &_::LDA, &_::ABS, 4}, {"LDX", &_::LDX, &_::ABS, 4}, {"???", &_::XXX, &_::IMM, 0}, 
            {"BCS", &_::BCS, &_::REL, 2}, {"LDA", &_::LDA, &_::YIZ, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"LDY", &_::LDY, &_::ZPX, 4}, {"LDA", &_::LDA, &_::ZPX, 4}, {"LDX", &_::LDX, &_::ZPY, 4}, {"???", &_::XXX, &_::IMM, 0}, {"CLV", &_::CLV, &_::IMP, 2}, {"LDA", &_::LDA, &_::ABY, 4}, {"TSX", &_::TSX, &_::IMP, 2}, {"???", &_::XXX, &_::IMM, 0}, {"LDY", &_::LDY, &_::ABX, 4}, {"LDA", &_::LDA, &_::ABX, 4}, {"LDX", &_::LDX, &_::ABY, 4}, {"???", &_::XXX, &_::IMM, 0}, 
            {"CPY", &_::CPY, &_::IMM, 2}, {"CMP", &_::CMP, &_::XIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"CPY", &_::CPY, &_::ZPG, 3}, {"CMP", &_::CMP, &_::ZPG, 3}, {"DEC", &_::DEC, &_::ZPG, 5}, {"???", &_::XXX, &_::IMM, 0}, {"INY", &_::INY, &_::IMP, 2}, {"CMP", &_::CMP, &_::IMM, 2}, {"DEX", &_::DEX, &_::IMP, 2}, {"???", &_::XXX, &_::IMM, 0}, {"CPY", &_::CPY, &_::ABS, 4}, {"CMP", &_::CMP, &_::ABS, 4}, {"DEC", &_::DEC, &_::ABS, 6}, {"???", &_::XXX, &_::IMM, 0}, 
            {"BNE", &_::BNE, &_::REL, 2}, {"CMP", &_::CMP, &_::YIZ, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"CMP", &_::CMP, &_::ZPX, 4}, {"DEC", &_::DEC, &_::ZPX, 6}, {"???", &_::XXX, &_::IMM, 0}, {"CLD", &_::CLD, &_::IMP, 2}, {"CMP", &_::CMP, &_::ABY, 4}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"CMP", &_::CMP, &_::ABX, 4}, {"DEC", &_::DEC, &_::ABX, 7}, {"???", &_::XXX, &_::IMM, 0}, 
            {"CPX", &_::CPX, &_::IMM, 2}, {"SBC", &_::SBC, &_::XIZ, 6}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"CPX", &_::CPX, &_::ZPG, 3}, {"SBC", &_::SBC, &_::ZPG, 3}, {"INC", &_::INC, &_::ZPG, 5}, {"???", &_::XXX, &_::IMM, 0}, {"INX", &_::INX, &_::IMP, 2}, {"SBC", &_::SBC, &_::IMM, 2}, {"NOP", &_::NOP, &_::IMP, 2}, {"???", &_::XXX, &_::IMM, 0}, {"CPX", &_::CPX, &_::ABS, 4}, {"SBC", &_::SBC, &_::ABS, 4}, {"INC", &_::INC, &_::ABS, 6}, {"???", &_::XXX, &_::IMM, 0},
            {"BEQ", &_::BEQ, &_::REL, 2}, {"SBC", &_::SBC, &_::YIZ, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMP, 0}, {"???", &_::XXX, &_::IMM, 0}, {"SBC", &_::SBC, &_::ZPX, 4}, {"INC", &_::INC, &_::ZPX, 6}, {"???", &_::XXX, &_::IMM, 0}, {"SED", &_::SED, &_::IMP, 2}, {"SBC", &_::SBC, &_::ABY, 5}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"???", &_::XXX, &_::IMM, 0}, {"SBC", &_::SBC, &_::ABX, 4}, {"INC", &_::INC, &_::ABX, 7}, {"???", &_::XXX, &_::IMM, 0},
        }};
    };
}

#endif