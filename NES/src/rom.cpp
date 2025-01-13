#include "rom.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "mapper.h"
#define DEBUG_ROM


/*
https://www.nesdev.org/wiki/INES

The format of the header is as follows:
Bytes 	Description
0-3 	Constant $4E $45 $53 $1A (ASCII "NES" followed by MS-DOS end-of-file)
4 	Size of PRG ROM in 16 KB units
5 	Size of CHR ROM in 8 KB units (value 0 means the board uses CHR RAM)
6 	Flags 6 – Mapper, mirroring, battery, trainer
7 	Flags 7 – Mapper, VS/Playchoice, NES 2.0
8 	Flags 8 – PRG-RAM size (rarely used extension)
9 	Flags 9 – TV system (rarely used extension)
10 	Flags 10 – TV system, PRG-RAM presence (unofficial, rarely used extension)
11-15 	Unused padding (should be filled with zero, but some rippers put their name across bytes 7-15) 


FLAGS 6
76543210
||||||||
|||||||+- Nametable arrangement: 0: vertical arrangement ("horizontal mirrored") (CIRAM A10 = PPU A11)
|||||||                          1: horizontal arrangement ("vertically mirrored") (CIRAM A10 = PPU A10)
||||||+-- 1: Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory
|||||+--- 1: 512-byte trainer at $7000-$71FF (stored before PRG data)
||||+---- 1: Alternative nametable layout
++++----- Lower nybble of mapper number

FLAGS 7
76543210
||||||||
|||||||+- VS Unisystem
||||||+-- PlayChoice-10 (8 KB of Hint Screen data stored after CHR data)
||||++--- If equal to 2, flags 8-15 are in NES 2.0 format
++++----- Upper nybble of mapper number

FLAGS 8
76543210
||||||||
++++++++- PRG RAM size

FLAGS 9
76543210
||||||||
|||||||+- TV system (0: NTSC; 1: PAL)
+++++++-- Reserved, set to zero

FLAGS 10
76543210
  ||  ||
  ||  ++- TV system (0: NTSC; 2: PAL; 1/3: dual compatible)
  |+----- PRG RAM ($6000-$7FFF) (0: present; 1: not present)
  +------ 0: Board has no bus conflicts; 1: Board has bus conflicts

*/

#define VERTICAL   = 0
#define HORIZONTAL = 1

namespace
{
    #pragma pack (push, 1)
    struct NES_ROM_Header
    {
        char name[4];
        u8   prg_size;
        u8   chr_size;
        u8   flags_6;
        u8   flags_7;
        u8   flags_8;
        u8   flags_9;
        u8   flags_10;
        char unused[5];
    };
    #pragma pack (pop)
}

NES_ROM::NES_ROM(const char* file_name)
: prg_memory {}
, chr_memory {}
{
    NES_ROM_Header header {};

    std::ifstream file (file_name);

    if (!file.is_open())
    {
        std::cerr << "File could not be opened" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    file.read(reinterpret_cast<char*> (&header), sizeof(header));

    prg_bank_n = header.prg_size;
    chr_bank_n = header.chr_size;

    mapper_id =  (header.flags_7  & 0xF0) | (header.flags_6 >> 4);
    mirror = static_cast<int> (header.flags_6 & 0x01);


    prg_memory.resize(prg_bank_n * 16384);
    chr_memory.resize((chr_bank_n == 0 ? 1 : chr_bank_n) * 8192);

    // checking if there is a trainer (idk what that is)
    if(header.flags_6 & 0x04)
        file.seekg(512, std::ios::cur);

    // if(((header.flags_7 >> 2) & 0x3) == 2)
    // {}


    file.read(reinterpret_cast<char*>(prg_memory.data()), prg_memory.size());
    file.read(reinterpret_cast<char*>(chr_memory.data()), chr_memory.size());

    switch (mapper_id)
    {
        case 0: mapper = std::make_shared<Mapper_000> (prg_bank_n, chr_bank_n);
        break;
        default: throw std::runtime_error("NO MAPPER");
        break;
    }

    file.close();

#ifdef DEBUG_ROM
    std::cout 
    << file_name << '\n'
    << std::format("file_format: {:}\n", (header.flags_7 >> 2) & 0x3)
    << std::format("prg_banks:   {:}\n", header.prg_size)
    << std::format("chr_banks:   {:}\n", header.chr_size)
    << std::format("mapper_id:   {:}\n", mapper_id)
    << std::format("flag_6:      {:0>8B}\n", header.flags_6)
    << std::format("flag_7:      {:0>8B}\n", header.flags_7);
#endif

}

bool NES_ROM::cpu_read (u16 address, u8& data)
{
    (void) address;
    (void) data;
    std::uint32_t mapped_address {};
    // std::vector<u8> prg_memory;
    return mapper->cpu_read(address, mapped_address, data);
    // return false;
}

/* GETTERS */
u8 NES_ROM::get_prg_bank_n () const {return prg_bank_n;}
u8 NES_ROM::get_chr_bank_n () const {return chr_bank_n;}

std::vector<u8>& NES_ROM::get_prg_memory () {return prg_memory;}
std::vector<u8>& NES_ROM::get_chr_memory () {return chr_memory;}





