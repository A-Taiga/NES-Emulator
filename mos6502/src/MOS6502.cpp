#include "MOS6502.h"
#include <iostream>
#include <format>
#include <cstring>

CPU::MOS6502::MOS6502 (read_cb read, write_cb write)
: read{read}
, write{write}
{}

void CPU::MOS6502::update (void)
{
    // TODO
}

/* GETTERS */
word CPU::MOS6502::get_PC              () const {return PC;}
byte CPU::MOS6502::get_AC              () const {return AC;}
byte CPU::MOS6502::get_X               () const {return X;}
byte CPU::MOS6502::get_Y               () const {return Y;}
byte CPU::MOS6502::get_SR              () const {return SR;}
byte CPU::MOS6502::get_SP              () const {return SP;}
word CPU::MOS6502::get_current_address () const {return current.address;}
byte CPU::MOS6502::get_current_data    () const {return current.data;}
int CPU::MOS6502::get_current_cycles   () const {return current.cycles;}

const _6502::Instruction& CPU::MOS6502::get_instruction (std::size_t index) {return instruction_table[index].ins;}

const CPU::MOS6502::Opcode* const CPU::MOS6502::get_current_ins () const {return current.ins;}

void CPU::MOS6502::set_flag(const Flag Flag, const bool condition)
{
    if (condition)
        SR |= static_cast <byte> (Flag);
    else
        SR &= ~static_cast <byte> (Flag);
}

void CPU::MOS6502::stack_push (const byte data)
{
    --SP;
    write (stk_begin + SP, data);
}

byte CPU::MOS6502::stack_pop (void)
{
    const auto result = read (stk_begin + SP);
    ++SP;
    return result;
}

/* 
    ADDRESSING MODES 

    some of these functions will return an extra cycle 
    if a page boundry was crossed
*/

// accumulator
int CPU::MOS6502::ACC (void)
{
    current.data = AC;
    return 0;
}

// absolute
int CPU::MOS6502::ABS (void)
{
    const byte low = read (PC++);
    const byte high = read (PC++);
    current.address = (high << 8) | low;
    return 0;
}

// absolute X
int CPU::MOS6502::ABX (void)
{
    const byte low = read (PC++);
    const byte high = read (PC++);
    current.address = ((high << 8) | low) + X;
    return (current.address & 0xFF00) != (high << 8) ? 1 : 0;
}

// absolute Y
int CPU::MOS6502::ABY (void)
{
    const byte low = read (PC++);
    const byte high = read (PC++);
    current.address = ((high << 8) | low) + Y;
    return (current.address & 0xFF00) != (high << 8) ? 1 : 0;
}

// # / immediate 
int CPU::MOS6502::IMM (void)
{
    current.address = PC++;
    return 0;
}

// implied
int CPU::MOS6502::IMP (void)
{
    // does nothing?
    return 0;
}

// indirect
int CPU::MOS6502::IND (void)
{
    const byte low = read (PC++);
    const byte high = read (PC++);
    current.address = (high << 8) | low;
    return 0;
}

// X-indexed indirect zeropage address
// operand is zeropage address; effective address is word in (LL + X, LL + X + 1), inc. without carry: C.w($00LL + X)
int CPU::MOS6502::XIZ (void)
{
    const byte temp = read (PC++);
    const byte low = read (temp + X);
    const byte high = read (temp + X + 1);
    current.address = (high << 8) | low;
    return 0;
}


// Y-indexed indirect zeropage address
// operand is zeropage address; effective address is word in (LL, LL + 1) incremented by Y with carry: C.w($00LL) + Y
int CPU::MOS6502::YIZ (void)
{
    const byte temp = read (PC++);
    const byte low = read (temp);
    const byte high = read (temp + 1);
    current.address = ((high << 8) | low) + Y;
    return (current.address & 0xFF00) != (high << 8) ? 1 : 0;
}

// relative
// branch target is PC + signed offset BB 
int CPU::MOS6502::REL (void)
{
    current.address = read (PC);
    current.address |= current.address & 0x80 ? 0xFF00 : 0x0000;
    return 0;
}

// zeropage
int CPU::MOS6502::ZPG (void)
{
    current.address = read (PC++);
    return 0;
}

// zeropage X-indexed
int CPU::MOS6502::ZPX (void)
{
    current.address = read (PC++) + X;
    return 0;
}

// zeropage Y-indexed
int CPU::MOS6502::ZPY (void)
{
    current.address = read (PC++) + Y;
    return 0;
}

/* OPCODES */

// break
void CPU::MOS6502::BRK (void)
{
    ++PC;

    stack_push (PC & 0xFF00);
    stack_push (PC & 0x00FF);

    set_flag (Flag::B, true);
    stack_push (SR);
    set_flag (Flag::B, false);

    set_flag (Flag::I, true);

    PC = read (0xFFFE) | (read (0xFFFF) << 8);
}

// bitwise OR
void CPU::MOS6502::ORA (void)
{
    AC |= read (current.address);
    set_flag (Flag::Z, AC == 0x00);
    set_flag (Flag::N, AC & 0x80);
}

// arithmetic shift left
void CPU::MOS6502::ASL (void)
{
    current.data = current.ins->mode == &MOS6502::ACC ? AC : read (current.address);
    set_flag (Flag::C, current.data * 0x80);
    current.data <<= 1;
    set_flag (Flag::Z, current.data == 0x00);
    set_flag (Flag::N, current.data & 0x80);
    if (current.ins->mode == &MOS6502::ACC)
        AC = current.data;
    else
        write (current.address, current.data);
}

// push processor status
void CPU::MOS6502::PHP (void)
{
    set_flag (Flag::B, true);
    set_flag (Flag::_, true);
    stack_push (SR);
    set_flag (Flag::B, false);
    set_flag (Flag::_, false);
}

// branch if plus
void CPU::MOS6502::BPL (void)
{
    if (!(static_cast <byte> (Flag::N) & SR))
    {
        // branch taken so add a cycle
        ++current.cycles;

        current.address += PC;

        // page boundry crossed
        if ((current.address & 0xFF00) != (PC & 0xFF00 ))
            ++current.cycles;
            
        PC = current.address;
    }
}

// clear carry
void CPU::MOS6502::CLC (void)
{
    SR &= ~static_cast <byte> (Flag::C);
}

// jump to subroutine
void CPU::MOS6502::JSR (void)
{
    ++PC;
    stack_push ((PC >> 8) & 0x00FF);
    stack_push (PC & 0x00FF);
    PC = current.address;
}

// bitwise AND
void CPU::MOS6502::AND (void)
{
    AC &= read (current.address);
    set_flag (Flag::Z, AC == 0x00);
    set_flag (Flag::N, AC & 0x80);
}

// bit test
void CPU::MOS6502::BIT (void)
{
    const byte temp = AC & read (current.address);
    
    set_flag (Flag::Z, temp == 0x00);
    set_flag (Flag::V, temp & 0x40);
    set_flag (Flag::N, temp & 0x80);
}

// rotate left
void CPU::MOS6502::ROL (void)
{
    current.data = current.ins->mode == &MOS6502::ACC ? AC : read (current.address);
    
    set_flag (Flag::C, current.data & 0x80);
    
    current.data <<= 1;
    current.data |= static_cast <byte> (Flag::C) & SP;
    
    set_flag (Flag::Z, current.data == 0x00);
    set_flag (Flag::N, current.data & 0x80);
    
    if (current.ins->mode == &MOS6502::ACC)
        AC = current.data;
    else
        write (current.address, current.data);
}

// pull processor status
void CPU::MOS6502::PLP (void)
{
    SR = stack_pop();
}

// branch if minus
void CPU::MOS6502::BMI (void)
{
    if (!(static_cast <byte> (Flag::N) & SR))
    {
        // branch taken so add cycle
        ++current.cycles;

        current.address += PC;

        // page boundry crossed
        if ((current.address & 0xFF00) != (PC & 0xFF00))
            ++current.cycles;

        PC = current.address;
    }
}

// set carry
void CPU::MOS6502::SEC (void)
{
    set_flag(Flag::C, true);
}

// return from interrupt
void CPU::MOS6502::RTI (void)
{
    SR = stack_pop();

    // these two flags are ignored when returning from the stack
    SR &= ~static_cast <byte> (Flag::B);
    SR &= ~static_cast <byte> (Flag::_);

    PC = stack_pop();
    PC |= stack_pop() << 8;
}

// bitwise exclusive OR
void CPU::MOS6502::EOR (void)
{
    AC ^= read (current.address);
    set_flag (Flag::Z, AC == 0x0);
    set_flag (Flag::N, AC & 0x80);
}

// logical shift right
void CPU::MOS6502::LSR (void)
{
    current.data = current.ins->mode == &MOS6502::ACC ? AC : read (current.address);
    set_flag (Flag::C, current.data & 0x01);
    current.data >>= 1;
    set_flag (Flag::Z, current.data == 0x00);
    set_flag (Flag::N, current.data & 0x80);
    if (current.ins->mode == &MOS6502::ACC)
        AC = current.data;
    else
        write (current.address, current.data);
}

// push accumulator
void CPU::MOS6502::PHA (void)
{
    stack_push (AC);
}

// jump
void CPU::MOS6502::JMP (void)
{
    PC = current.address;
}

// branch if overflow clear
void CPU::MOS6502::BVC (void)
{
    if (!(static_cast <byte> (Flag::V) & SR))
    {
        // branching requires an additional cycle
        ++current.cycles;

        current.address += PC;

        // page boundry check
        if ((current.address & 0x00FF) != (PC & 0xFF00))
            ++current.cycles;

        PC = current.address;
    }
}

// clear interrupt disable
void CPU::MOS6502::CLI (void)
{
    set_flag (Flag::I, false);
}

// return from subroutinef
void CPU::MOS6502::RTS (void)
{
    const byte low = stack_pop();
    const byte high = stack_pop();
    PC = (high << 8) | low;
    ++PC;
}

// pull accumulator
void CPU::MOS6502::PLA (void)
{
    AC = stack_pop();
    set_flag (Flag::Z, AC == 0x00);
    set_flag (Flag::N, AC & 0x80);
}

// add with carry
void CPU::MOS6502::ADC (void)
{
    current.data = read (current.address);

    const word result = AC + current.data + (static_cast <byte> (Flag::C) & SR);
    
    set_flag (Flag::C, (result & 0xFF00) != 0);
    set_flag (Flag::Z, result == 0);
    set_flag (Flag::V, ~(result ^ AC) & (result ^ current.data) & 0x0080);
    set_flag (Flag::N, result & 0x0080);
    
    AC = result & 0x00FF;
}

// rotate right
void CPU::MOS6502::ROR (void)
{
    current.data = current.ins->mode == &MOS6502::ACC ? AC : read (current.address);

    set_flag (Flag::C, current.data & 0x80);
    
    current.data >>= 1;
    current.data |= (static_cast <byte> (Flag::C) & SP) << 7;
    
    set_flag (Flag::Z, current.data == 0x00);
    set_flag (Flag::N, current.data & 0x80);

    if (current.ins->mode == &MOS6502::ACC)
        AC = current.data;
    else
        write (current.address, current.data);
}

// branch if overflow set
void CPU::MOS6502::BVS (void)
{
    if (SR & static_cast <byte> (Flag::V))
    {
        // branch taken cycles added
        ++current.cycles;
        
        current.address += PC;

        // page boundry crossed
        if ((current.address & 0xFF00) != (PC & 0xFF00))
            ++current.cycles;

        PC = current.address;
    }
}

// set interrupt disable
void CPU::MOS6502::SEI (void)
{
    set_flag (Flag::I, true);
}

void CPU::MOS6502::STA (void)
{
    write (current.address, AC);
}

// store Y
void CPU::MOS6502::STY (void)
{
    write (current.address, Y);
}

// store X
void CPU::MOS6502::STX (void)
{
    write (current.address, X);
}

// decrement Y
void CPU::MOS6502::DEY (void)
{
    --Y;
    set_flag (Flag::Z, Y == 0x00);
    set_flag (Flag::N, Y & 0x80);
}

// transfer X to accumulator
void CPU::MOS6502::TXA (void)
{
    AC = X;
    set_flag (Flag::Z, AC == 0x00);
    set_flag (Flag::N, AC & 0x80);
}

// branch if carry clear
void CPU::MOS6502::BCC (void)
{
    if (!(SR & static_cast <byte> (Flag::C)))
    {
        // branch taken
        ++current.cycles;

        current.address += PC;

        // page boundry crossed
        if ((current.address & 0xFF00) != (PC & 0xFF00))
            ++current.cycles;

        PC = current.address;
    }
}

// transfer Y to accumulator
void CPU::MOS6502::TYA (void)
{
    AC = Y;
    set_flag (Flag::Z, AC == 0x00);
    set_flag (Flag::N, AC & 0x80);
}

// transfer X to stack pointer
void CPU::MOS6502::TXS (void)
{
    SP = X;
}

// load Y
void CPU::MOS6502::LDY (void)
{
    Y = read (current.address);
    set_flag (Flag::Z, Y == 0x00);
    set_flag (Flag::N, Y & 0x80);
}

// load accumulator
void CPU::MOS6502::LDA (void)
{
    AC = read (current.address);
    set_flag (Flag::Z, AC == 0x00);
    set_flag (Flag::N, AC & 0x80);
}

// load X
void CPU::MOS6502::LDX (void)
{
    X = read (current.address);
    set_flag (Flag::Z, X == 0x00);
    set_flag (Flag::N, X & 0x80);
}

// transfer accumulator to Y
void CPU::MOS6502::TAY (void)
{
    Y = AC;
    set_flag (Flag::Z, Y == 0x00);
    set_flag (Flag::N, Y & 0x80);
}

// transfer accumulator to X
void CPU::MOS6502::TAX (void)
{
    X = AC;
    set_flag (Flag::Z, X == 0x00);
    set_flag (Flag::N, X & 0x80);
}

// branch if carry set
void CPU::MOS6502::BCS (void)
{
    if (SR & static_cast <byte> (Flag::C))
    {
        // branch taken cycles added
        ++current.cycles;
        
        current.address += PC;

        // page boundry crossed
        if ((current.address & 0xFF00) != (PC & 0xFF00))
            ++current.cycles;

        PC = current.address;
    }
}

// clear overflow
void CPU::MOS6502::CLV (void)
{
    set_flag (Flag::V, false);
}

// transfer stack pointer to X
void CPU::MOS6502::TSX (void)
{
    X = SP;
    set_flag (Flag::Z, X == 0x00);
    set_flag (Flag::N, X & 0x80);
}

// compare Y
void CPU::MOS6502::CPY (void)
{
    current.data = read (current.address);

    set_flag (Flag::C, Y >= current.data);
    set_flag (Flag::Z, Y == 0x00);
    set_flag (Flag::N, (Y - current.data) & 0x80);
}

// compare accumulator
void CPU::MOS6502::CMP (void)
{
    current.data = read (current.address);

    set_flag (Flag::C, AC >= current.data);
    set_flag (Flag::Z, AC == 0x00);
    set_flag (Flag::N, (AC - current.data) & 0x80);
}

// decrement memory
void CPU::MOS6502::DEC (void)
{
    current.data = read (current.address);
    
    --current.data;

    set_flag (Flag::Z, current.data == 0x00);
    set_flag (Flag::N, current.data & 0x80);

    write (current.address, current.data);
}

// increment Y
void CPU::MOS6502::INY (void)
{
    ++Y;
    
    set_flag (Flag::Z, Y == 0x0);
    set_flag (Flag::N, Y & 0x80);
}

// decrement X
void CPU::MOS6502::DEX (void)
{
    --X;
    
    set_flag (Flag::Z, X == 0x0);
    set_flag (Flag::N, X & 0x80);
}

// branch if not equal
void CPU::MOS6502::BNE (void)
{
    if (!(SR & static_cast <byte> (Flag::Z)))
    {
        // branch taken cycles added
        ++current.cycles;
        
        current.address += PC;

        // page boundry crossed
        if ((current.address & 0xFF00) != (PC & 0xFF00))
            ++current.cycles;

        PC = current.address;
    }
}

// clear decimal
void CPU::MOS6502::CLD (void)
{
    set_flag(Flag::D, false);
}

// compare X
void CPU::MOS6502::CPX (void)
{
    current.data = read (current.address);

    set_flag (Flag::C, X >= current.data);
    set_flag (Flag::Z, X == current.data);
    set_flag (Flag::N, (X - current.data) & 0x80);
}

// subtract with carry
void CPU::MOS6502::SBC (void)
{
    current.data = read (current.address);

    const word result = AC + ~current.data + (static_cast <byte> (Flag::C) & SR);

    set_flag (Flag::C, ~(result < 0x00));
    set_flag (Flag::Z, result == 0x00);
    set_flag (Flag::V, (result ^ AC) & (result ^ ~current.data) & 0x80);
    set_flag (Flag::N, result & 0x80);

    AC = result & 0x00FF;
}

// increment memory
void CPU::MOS6502::INC (void)
{
    current.data = read (current.address);
    
    ++current.data;
   
    set_flag (Flag::Z, current.data == 0x00);
    set_flag (Flag::N, current.data & 0x80);

    write (current.address, current.data);
}

// increment X
void CPU::MOS6502::INX (void)
{
    ++X;
    
    set_flag (Flag::Z, X == 0x00);
    set_flag (Flag::N, X & 0x80);
}

void CPU::MOS6502::NOP (void)
{}

void CPU::MOS6502::BEQ (void)
{
    if (SR & static_cast <byte> (Flag::Z))
    {
        // branch taken cycles added
        ++current.cycles;
        
        current.address += PC;

        // page boundry crossed
        if ((current.address & 0xFF00) != (PC & 0xFF00))
            ++current.cycles;

        PC = current.address;
    }
}

// set decimal
void CPU::MOS6502::SED (void)
{
    set_flag (Flag::D, true);
}

// empty instruction (illegal)
void CPU::MOS6502::XXX (void)
{

}