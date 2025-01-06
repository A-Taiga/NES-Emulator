#ifndef MOS6502_H
#define MOS6502_H

#include "utility.h"
#include <array>
#include <functional>
#include "mos6502_instructions.h"

/*

https://www.nesdev.org/wiki/Instruction_reference

https://www.masswerk.at/6502/6502_instruction_set.html#BVC

data is stored in little-endien (LLHH)

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
            _6502::Instruction ins;
            void (MOS6502::*opcode)(void);
            int (MOS6502::*mode)(void);
            int cycles;
        };

    public:

        using write_cb = std::function <void(const word, const byte)>;
        using read_cb = std::function <byte(const word)>;

        MOS6502 (read_cb, write_cb);

        void update (void);

        /* GETTERS FOR DEBUG */
        word get_PC              () const;
        byte get_AC              () const;
        byte get_X               () const;
        byte get_Y               () const;
        byte get_SR              () const;
        byte get_SP              () const;
        word get_current_address () const;
        byte get_current_data    () const;
        int get_current_cycles   () const;

        const Opcode*  get_current_ins () const;
        
        static const _6502::Instruction& get_instruction (const word index);

    private:

        static constexpr word stk_begin = 0x0100;

        /* READ WRITE CALLBACKS */
        read_cb  read;
        write_cb write;

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
        byte stack_pop (void);

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
        using Inst = _6502::Inst;
        using Mode = _6502::Mode;
        static constexpr std::array<Opcode, 256> instruction_table
        {{
            {{Inst::BRK, Mode::IMP}, &_::BRK, &_::IMP, 7}, {{Inst::ORA, Mode::XIZ}, &_::ORA, &_::XIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::ORA, Mode::ZPG}, &_::ORA, &_::ZPG, 3}, {{Inst::ASL, Mode::ZPG}, &_::ASL, &_::ZPG, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::PHP, Mode::IMP}, &_::PHP, &_::IMP, 3}, {{Inst::ORA, Mode::IMM}, &_::ORA, &_::IMM, 2}, {{Inst::ASL, Mode::ACC}, &_::ASL, &_::ACC, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::ORA, Mode::ABS}, &_::ORA, &_::ABS, 4}, {{Inst::ASL, Mode::ABS}, &_::ASL, &_::ABS, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::BPL, Mode::REL}, &_::BPL, &_::REL, 2}, {{Inst::ORA, Mode::YIZ}, &_::ORA, &_::YIZ, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::ORA, Mode::ZPX}, &_::ORA, &_::ZPX, 4}, {{Inst::ASL, Mode::ZPX}, &_::ASL, &_::ZPX, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CLC, Mode::IMP}, &_::CLC, &_::IMP, 2}, {{Inst::ORA, Mode::ABY}, &_::ORA, &_::ABY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::ORA, Mode::ABX}, &_::ORA, &_::ABX, 4}, {{Inst::ASL, Mode::ABX}, &_::ASL, &_::ABX, 7}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::JSR, Mode::ABS}, &_::JSR, &_::ABS, 6}, {{Inst::AND, Mode::XIZ}, &_::AND, &_::XIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::BIT, Mode::ZPG}, &_::BIT, &_::ZPG, 3}, {{Inst::AND, Mode::ZPG}, &_::AND, &_::ZPG, 3}, {{Inst::ROL, Mode::ZPG}, &_::ROL, &_::ZPG, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::PLP, Mode::IMP}, &_::PLP, &_::IMP, 4}, {{Inst::AND, Mode::IMM}, &_::AND, &_::IMM, 2}, {{Inst::ROL, Mode::ACC}, &_::ROL, &_::ACC, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::BIT, Mode::IMP}, &_::BIT, &_::ABS, 4}, {{Inst::AND, Mode::ABS}, &_::AND, &_::ABS, 4}, {{Inst::ROL, Mode::ABS}, &_::ROL, &_::ABS, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::BMI, Mode::REL}, &_::BMI, &_::REL, 2}, {{Inst::AND, Mode::YIZ}, &_::AND, &_::YIZ, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::AND, Mode::ZPX}, &_::AND, &_::ZPX, 4}, {{Inst::ROL, Mode::ZPX}, &_::ROL, &_::ZPX, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::SEC, Mode::IMP}, &_::SEC, &_::IMP, 2}, {{Inst::AND, Mode::ABY}, &_::AND, &_::ABY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::AND, Mode::ABX}, &_::AND, &_::ABX, 4}, {{Inst::ROL, Mode::ABX}, &_::ROL, &_::ABX, 7}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::RTI, Mode::IMP}, &_::RTI, &_::IMP, 6}, {{Inst::EOR, Mode::XIZ}, &_::EOR, &_::XIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::EOR, Mode::ZPG}, &_::EOR, &_::ZPG, 3}, {{Inst::LSR, Mode::ZPG}, &_::LSR, &_::ZPG, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::PHA, Mode::IMP}, &_::PHA, &_::IMP, 3}, {{Inst::EOR, Mode::IMM}, &_::EOR, &_::IMM, 2}, {{Inst::LSR, Mode::ACC}, &_::LSR, &_::ACC, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::JMP, Mode::IMP}, &_::JMP, &_::ABS, 3}, {{Inst::EOR, Mode::ABS}, &_::EOR, &_::ABS, 4}, {{Inst::LSR, Mode::ABS}, &_::LSR, &_::ABS, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::BVC, Mode::REL}, &_::BVC, &_::REL, 2}, {{Inst::EOR, Mode::YIZ}, &_::EOR, &_::YIZ, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::EOR, Mode::ZPX}, &_::EOR, &_::ZPX, 4}, {{Inst::LSR, Mode::ZPX}, &_::LSR, &_::ZPX, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CLI, Mode::IMP}, &_::CLI, &_::IMP, 2}, {{Inst::EOR, Mode::ABY}, &_::EOR, &_::ABY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::EOR, Mode::ABX}, &_::EOR, &_::ABX, 4}, {{Inst::LSR, Mode::ABX}, &_::LSR, &_::ABX, 7}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::RTS, Mode::IMP}, &_::RTS, &_::IMP, 6}, {{Inst::ADC, Mode::XIZ}, &_::ADC, &_::XIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::ADC, Mode::ZPG}, &_::ADC, &_::ZPG, 3}, {{Inst::ROR, Mode::ZPG}, &_::ROR, &_::ZPG, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::PLA, Mode::IMP}, &_::PLA, &_::IMP, 4}, {{Inst::ADC, Mode::IMM}, &_::ADC, &_::IMM, 2}, {{Inst::ROR, Mode::ACC}, &_::ROR, &_::ACC, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::JMP, Mode::IMP}, &_::JMP, &_::IND, 5}, {{Inst::ADC, Mode::ABS}, &_::ADC, &_::ABS, 4}, {{Inst::ROR, Mode::ABS}, &_::ROR, &_::ABS, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::BVS, Mode::REL}, &_::BVS, &_::REL, 2}, {{Inst::ADC, Mode::YIZ}, &_::ADC, &_::YIZ, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::ADC, Mode::ZPX}, &_::ADC, &_::ZPX, 4}, {{Inst::ROR, Mode::ZPX}, &_::ROR, &_::ZPX, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::SEI, Mode::IMP}, &_::SEI, &_::IMP, 2}, {{Inst::ADC, Mode::ABY}, &_::ADC, &_::ABY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::ADC, Mode::ABX}, &_::ADC, &_::ABX, 4}, {{Inst::ROR, Mode::ABX}, &_::ROR, &_::ABX, 7}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::STA, Mode::XIZ}, &_::STA, &_::XIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::STY, Mode::ZPG}, &_::STY, &_::ZPG, 3}, {{Inst::STA, Mode::ZPG}, &_::STA, &_::ZPG, 3}, {{Inst::STX, Mode::ZPG}, &_::STX, &_::ZPG, 3}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::DEY, Mode::IMP}, &_::DEY, &_::IMP, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::TXA, Mode::IMP}, &_::TXA, &_::IMP, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::STY, Mode::ABS}, &_::STY, &_::ABS, 4}, {{Inst::STA, Mode::ABS}, &_::STA, &_::ABS, 4}, {{Inst::STX, Mode::ABS}, &_::STX, &_::ABS, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::BCC, Mode::REL}, &_::BCC, &_::REL, 2}, {{Inst::STA, Mode::YIZ}, &_::STA, &_::YIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::STY, Mode::ZPX}, &_::STY, &_::ZPX, 4}, {{Inst::STA, Mode::ZPX}, &_::STA, &_::ZPX, 4}, {{Inst::STX, Mode::ZPY}, &_::STX, &_::ZPY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::TYA, Mode::IMP}, &_::TYA, &_::IMP, 2}, {{Inst::STA, Mode::ABY}, &_::STA, &_::ABY, 5}, {{Inst::TXS, Mode::IMP}, &_::TXS, &_::IMP, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::STA, Mode::ABX}, &_::STA, &_::ABX, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::LDY, Mode::IMM}, &_::LDY, &_::IMM, 2}, {{Inst::LDA, Mode::XIZ}, &_::LDA, &_::XIZ, 6}, {{Inst::LDX, Mode::IMM}, &_::LDX, &_::IMM, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::LDY, Mode::ZPG}, &_::LDY, &_::ZPG, 3}, {{Inst::LDA, Mode::ZPG}, &_::LDA, &_::ZPG, 3}, {{Inst::LDX, Mode::ZPG}, &_::LDX, &_::ZPG, 3}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::TAY, Mode::IMP}, &_::TAY, &_::IMP, 2}, {{Inst::LDA, Mode::IMM}, &_::LDA, &_::IMM, 2}, {{Inst::TAX, Mode::IMP}, &_::TAX, &_::IMP, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::LDY, Mode::ABS}, &_::LDY, &_::ABS, 4}, {{Inst::LDA, Mode::ABS}, &_::LDA, &_::ABS, 4}, {{Inst::LDX, Mode::ABS}, &_::LDX, &_::ABS, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::BCS, Mode::REL}, &_::BCS, &_::REL, 2}, {{Inst::LDA, Mode::YIZ}, &_::LDA, &_::YIZ, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::LDY, Mode::ZPX}, &_::LDY, &_::ZPX, 4}, {{Inst::LDA, Mode::ZPX}, &_::LDA, &_::ZPX, 4}, {{Inst::LDX, Mode::ZPY}, &_::LDX, &_::ZPY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CLV, Mode::IMP}, &_::CLV, &_::IMP, 2}, {{Inst::LDA, Mode::ABY}, &_::LDA, &_::ABY, 4}, {{Inst::TSX, Mode::IMP}, &_::TSX, &_::IMP, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::LDY, Mode::ABX}, &_::LDY, &_::ABX, 4}, {{Inst::LDA, Mode::ABX}, &_::LDA, &_::ABX, 4}, {{Inst::LDX, Mode::ABY}, &_::LDX, &_::ABY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::CPY, Mode::IMM}, &_::CPY, &_::IMM, 2}, {{Inst::CMP, Mode::XIZ}, &_::CMP, &_::XIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CPY, Mode::ZPG}, &_::CPY, &_::ZPG, 3}, {{Inst::CMP, Mode::ZPG}, &_::CMP, &_::ZPG, 3}, {{Inst::DEC, Mode::ZPG}, &_::DEC, &_::ZPG, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::INY, Mode::IMP}, &_::INY, &_::IMP, 2}, {{Inst::CMP, Mode::IMM}, &_::CMP, &_::IMM, 2}, {{Inst::DEX, Mode::IMP}, &_::DEX, &_::IMP, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CPY, Mode::IMP}, &_::CPY, &_::ABS, 4}, {{Inst::CMP, Mode::ABS}, &_::CMP, &_::ABS, 4}, {{Inst::DEC, Mode::ABS}, &_::DEC, &_::ABS, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::BNE, Mode::REL}, &_::BNE, &_::REL, 2}, {{Inst::CMP, Mode::YIZ}, &_::CMP, &_::YIZ, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CMP, Mode::ZPX}, &_::CMP, &_::ZPX, 4}, {{Inst::DEC, Mode::ZPX}, &_::DEC, &_::ZPX, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CLD, Mode::IMP}, &_::CLD, &_::IMP, 2}, {{Inst::CMP, Mode::ABY}, &_::CMP, &_::ABY, 4}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CMP, Mode::ABX}, &_::CMP, &_::ABX, 4}, {{Inst::DEC, Mode::ABX}, &_::DEC, &_::ABX, 7}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, 
            {{Inst::CPX, Mode::IMM}, &_::CPX, &_::IMM, 2}, {{Inst::SBC, Mode::XIZ}, &_::SBC, &_::XIZ, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CPX, Mode::ZPG}, &_::CPX, &_::ZPG, 3}, {{Inst::SBC, Mode::ZPG}, &_::SBC, &_::ZPG, 3}, {{Inst::INC, Mode::ZPG}, &_::INC, &_::ZPG, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::INX, Mode::IMP}, &_::INX, &_::IMP, 2}, {{Inst::SBC, Mode::IMM}, &_::SBC, &_::IMM, 2}, {{Inst::NOP, Mode::IMP}, &_::NOP, &_::IMP, 2}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::CPX, Mode::IMP}, &_::CPX, &_::ABS, 4}, {{Inst::SBC, Mode::ABS}, &_::SBC, &_::ABS, 4}, {{Inst::INC, Mode::ABS}, &_::INC, &_::ABS, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0},
            {{Inst::BEQ, Mode::REL}, &_::BEQ, &_::REL, 2}, {{Inst::SBC, Mode::YIZ}, &_::SBC, &_::YIZ, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::SBC, Mode::ZPX}, &_::SBC, &_::ZPX, 4}, {{Inst::INC, Mode::ZPX}, &_::INC, &_::ZPX, 6}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::SED, Mode::IMP}, &_::SED, &_::IMP, 2}, {{Inst::SBC, Mode::ABY}, &_::SBC, &_::ABY, 5}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0}, {{Inst::SBC, Mode::ABX}, &_::SBC, &_::ABX, 4}, {{Inst::INC, Mode::ABX}, &_::INC, &_::ABX, 7}, {{Inst::XXX, Mode::IMP}, &_::XXX, &_::IMP, 0},
        }};
    };
}

#endif