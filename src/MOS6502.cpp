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
        else if (current.mode == &_::IDX) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:02X}", index, read(index), read(index+1), current.mnemonic, read(index+1));
            index += 2;
        }
        else if (current.mode == &_::IDY) 
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

/* ADDRESSING MODES */

int CPU::MOS6502::ACC (void)
{
    return 0;
}

int CPU::MOS6502::ABS (void)
{
    return 0;
}

int CPU::MOS6502::ABX (void)
{
    return 0;
}

int CPU::MOS6502::ABY (void)
{
    return 0;
}

int CPU::MOS6502::IMM (void)
{
    return 0;
}

int CPU::MOS6502::IMP (void)
{
    return 0;
}

int CPU::MOS6502::IND (void)
{
    return 0;
}

int CPU::MOS6502::IDX (void)
{
    return 0;
}

int CPU::MOS6502::IDY (void)
{
    return 0;
}

int CPU::MOS6502::REL (void)
{
    return 0;
}

int CPU::MOS6502::ZPG (void)
{
    return 0;
}

int CPU::MOS6502::ZPX (void)
{
    return 0;
}

int CPU::MOS6502::ZPY (void)
{
    return 0;
}

/* OPCODES */

// break
void CPU::MOS6502::BRK (void)
{
    PC += 2;
    stack_push ((PC >> 8) & 0x00FF);
    stack_push (PC & 0x00FF);
    set_flag(Flag::B, true);
    stack_push (SR);
    set_flag(Flag::B, false);
    set_flag(Flag::I, true);
    PC = static_cast <word> (read(0xFFFE)) | static_cast <word> (read(0xFFFF) << 8);
}

// bitwise OR
void CPU::MOS6502::ORA (void)
{
    AC |= read(current.address);
    set_flag(Flag::Z, AC == 0);
    set_flag(Flag::N, AC & 0x80);
}

// arithmetic shift left
void CPU::MOS6502::ASL (void)
{
    const word result = static_cast <word> (current.ins->mode == &MOS6502::IMP ? AC : read (current.address)) << 1;
    set_flag (Flag::C, (result & 0xFF00) != 0);
    set_flag (Flag::Z, (result & 0x00FF) == 0);
    set_flag (Flag::N, result & 0x0080);
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
    set_flag (Flag::Z, AC == 0);
    set_flag (Flag::N, AC & 0x80);
}

// bit test
void CPU::MOS6502::BIT (void)
{
    const byte temp = AC & read (current.address);
    set_flag (Flag::Z, temp == 0);
    set_flag (Flag::V, temp & 0x40);
    set_flag (Flag::N, temp & 0x80);
}

// rotate left
void CPU::MOS6502::ROL (void)
{
    const word result = (static_cast <word> ([&] ()
    {
        if (current.ins->mode == &MOS6502::IMP)
            return AC;
        else
            return read (current.address);
    }()) << 1) | (static_cast <byte> (Flag::C) & SP);

    set_flag (Flag::N, result & 0x0080);
    set_flag (Flag::Z, (result & 0x00FF) == 0);
    set_flag (Flag::C, (result & 0xFF00) != 0);


    if (current.ins->mode == &MOS6502::IMP)
        AC = result;
    else
        write (current.address, result & 0x00FF);
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

void CPU::MOS6502::EOR (void)
{

}

void CPU::MOS6502::LSR (void)
{

}

void CPU::MOS6502::PHA (void)
{

}

void CPU::MOS6502::JMP (void)
{

}

void CPU::MOS6502::BVC (void)
{

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