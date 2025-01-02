#include "MOS6502.h"
#include <iostream>
#include <format>
#include <cstring>

CPU::MOS6502::MOS6502 (read_cb read, write_cb write)
: bus_read{read}
, bus_write{write}
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
        const Opcode& current = instruction_set[bus_read(index)];

        if (current.mode == &_::IMP) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:>9}", index, bus_read(index), current.mnemonic);
            index += 1;
        }
        else if (current.mode == &_::IMM) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} #${:02X}", index, bus_read(index), bus_read(index+1), current.mnemonic, bus_read(index+1));
            index += 2;
        }
        else if (current.mode == &_::ABS) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:} ${:04X} ", index, bus_read(index), bus_read(index+1), bus_read(index+2), current.mnemonic, (bus_read(index+2) << 8) | bus_read(index+1));
            index += 3;
        }
        else if (current.mode == &_::ZPG) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:02X}", index, bus_read(index), bus_read(index+1), current.mnemonic, bus_read(index+1));
            index += 2;
        }
        else if (current.mode == &_::ABX) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:} ${:04X}, X", index, bus_read(index), bus_read(index+1), bus_read(index+2), current.mnemonic, (bus_read(index+2) << 8) | bus_read(index+1));
            index += 3;
        }
        else if (current.mode == &_::ABY) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:}", index, bus_read(index), bus_read(index+1), bus_read(index+2), current.mnemonic);
            index += 3;
        }
        else if (current.mode == &_::ZPX) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:>6} ${:02X}", index, bus_read(index+1), current.mnemonic, bus_read(index+1));
            index += 2;
        }
        else if (current.mode == &_::ZPY) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6}", index, bus_read(index), bus_read(index+1), current.mnemonic);
            index += 2;
        }
        else if (current.mode == &_::IND) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:02X} {:}", index, bus_read(index), bus_read(index+1), bus_read(index+2), current.mnemonic);
            index += 3;
        }
        else if (current.mode == &_::IDX) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:02X}", index, bus_read(index), bus_read(index+1), current.mnemonic, bus_read(index+1));
            index += 2;
        }
        else if (current.mode == &_::IDY) 
        { 
            std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:02X}", index, bus_read(index), bus_read(index+1), current.mnemonic, bus_read(index+1));
            index += 2;
        }
        else if (current.mode == &_::REL)
        {
            if (std::strcmp(current.mnemonic.c_str(), "CPX"))
            {
                word result = bus_read (index+1);
                if (result & 0x80) 
                    result |= 0xFF00;
                word addr = index+2 + result;
                std::cout << std::format ("{:04X}: {:02X} {:02X} {:>6} ${:04X}", index, bus_read(index), bus_read(index+1), current.mnemonic, addr); 
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

void CPU::MOS6502::BRK (void)
{
    
}

void CPU::MOS6502::ORA (void)
{

}

void CPU::MOS6502::ASL (void)
{

}

void CPU::MOS6502::PHP (void)
{

}

void CPU::MOS6502::BPL (void)
{

}

void CPU::MOS6502::CLC (void)
{

}

void CPU::MOS6502::JSR (void)
{

}

void CPU::MOS6502::AND (void)
{

}

void CPU::MOS6502::BIT (void)
{

}

void CPU::MOS6502::ROL (void)
{

}

void CPU::MOS6502::PLP (void)
{

}

void CPU::MOS6502::BMI (void)
{

}

void CPU::MOS6502::SEC (void)
{

}

void CPU::MOS6502::RTI (void)
{

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

