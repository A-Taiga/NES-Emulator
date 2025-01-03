#include "MOS6502.h"
#include <iostream>
#include <format>
#include <cstring>



CPU::MOS6502::MOS6502 (read_cb read, write_cb write)
: read{read}
, write{write}
{}

void CPU::MOS6502::print_instruction_set (void) const
{
    std::cout << "   ";
    for(int i = 0; i < 16; ++i)
        std::cout << std::format(" {:X} ", i) << " ";

    int v = 0;
    for (auto i = 0; i < 256; ++i)
    {
        const auto& m = instruction_set[i].mnemonic;

                
        if(i % 16 == 0)
        {
            std::cout << '\n' << std::format("{:X}  ", v);
            ++v;
        }

        if(m == "???")
            std::cout << "___" << " ";
        else
            std::cout << m << " ";
    }
}

std::string CPU::MOS6502::get_opcode (const byte instruction) const
{
    return instruction_set[instruction].mnemonic;
}

void CPU::MOS6502::decompile (const word mem_size) 
{
    word index = 0;
    while (index < mem_size)
    {
        const Opcode& current = instruction_set[read(index)];

        if (current.mode == &_::IMP) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:>9}", index, read(index), current.mnemonic);
            index += 1;
        }
        else if (current.mode == &_::IMM) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} #${:02X}", index, read(index), read(index+1), current.mnemonic, read(index+1));
            index += 2;
        }
        else if (current.mode == &_::ABS) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:} ${:04X} ", index, read(index), read(index+1), read(index+2), current.mnemonic, (read(index+2) << 8) | read(index+1));
            index += 3;
        }
        else if (current.mode == &_::ZPG) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:02X}", index, read(index), read(index+1), current.mnemonic, read(index+1));
            index += 2;
        }
        else if (current.mode == &_::ABX) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:} ${:04X}, X", index, read(index), read(index+1), read(index+2), current.mnemonic, (read(index+2) << 8) | read(index+1));
            index += 3;
        }
        else if (current.mode == &_::ABY) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:}", index, read(index), read(index+1), read(index+2), current.mnemonic);
            index += 3;
        }
        else if (current.mode == &_::ZPX) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:>6} ${:02X}", index, read(index+1), current.mnemonic, read(index+1));
            index += 2;
        }
        else if (current.mode == &_::ZPY) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6}", index, read(index), read(index+1), current.mnemonic);
            index += 2;
        }
        else if (current.mode == &_::IND) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:}", index, read(index), read(index+1), read(index+2), current.mnemonic);
            index += 3;
        }
        else if (current.mode == &_::XIZ) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:02X}", index, read(index), read(index+1), current.mnemonic, read(index+1));
            index += 2;
        }
        else if (current.mode == &_::YIZ) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:02X}", index, read(index), read(index+1), current.mnemonic, read(index+1));
            index += 2;
        }
        else if (current.mode == &_::REL)
        {
            if (std::strcmp(current.mnemonic.c_str(), "CPX"))
            {
                word result = read (index+1);
                if (result & 0x80) 
                    result |= 0xFF00;
                word addr = index+2 + result;
                std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:04X}", index, read(index), read(index+1), current.mnemonic, addr); 
                index += 2;
            }
        }
        else {
            std::cout << std::format("{:04X}", index);
            index += 1;
        }
        std::cout << '\n';
    }
}

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

u8 CPU::MOS6502::stack_pop (void)
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
    // TODO
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
    // TODO
}

// set carry
void CPU::MOS6502::SEC (void)
{
    set_flag(Flag::C, true);
}

// return from interrupt
void CPU::MOS6502::RTI (void)
{
    // TODO
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

void CPU::MOS6502::CLI (void)
{

}

void CPU::MOS6502::RTS (void)
{

}

void CPU::MOS6502::PLA (void)
{

}

void CPU::MOS6502::ADC (void)
{

}

void CPU::MOS6502::ROR (void)
{

}

void CPU::MOS6502::BVS (void)
{

}

void CPU::MOS6502::SEI (void)
{

}

void CPU::MOS6502::STA (void)
{

}

void CPU::MOS6502::STY (void)
{

}

void CPU::MOS6502::STX (void)
{

}

void CPU::MOS6502::DEY (void)
{

}

void CPU::MOS6502::TXA (void)
{

}

void CPU::MOS6502::BCC (void)
{

}

void CPU::MOS6502::TYA (void)
{

}

void CPU::MOS6502::TXS (void)
{

}

void CPU::MOS6502::LDY (void)
{

}

void CPU::MOS6502::LDA (void)
{

}

void CPU::MOS6502::LDX (void)
{

}

void CPU::MOS6502::TAY (void)
{

}

void CPU::MOS6502::TAX (void)
{

}

void CPU::MOS6502::BCS (void)
{

}

void CPU::MOS6502::CLV (void)
{

}

void CPU::MOS6502::TSX (void)
{

}

void CPU::MOS6502::CPY (void)
{

}

void CPU::MOS6502::CMP (void)
{

}

void CPU::MOS6502::DEC (void)
{

}

void CPU::MOS6502::INY (void)
{

}

void CPU::MOS6502::DEX (void)
{

}

void CPU::MOS6502::BNE (void)
{

}

void CPU::MOS6502::CLD (void)
{

}

void CPU::MOS6502::CPX (void)
{

}

void CPU::MOS6502::SBC (void)
{

}

void CPU::MOS6502::INC (void)
{

}

void CPU::MOS6502::INX (void)
{

}

void CPU::MOS6502::NOP (void)
{

}

void CPU::MOS6502::BEQ (void)
{

}

void CPU::MOS6502::SED (void)
{

}

// empty instruction (illegal)
void CPU::MOS6502::XXX (void)
{

}