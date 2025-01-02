#include "mapper.h"

Mapper::Mapper (u8 _prg_banks, u8 _chr_banks)
: prg_banks {_prg_banks}
, chr_banks {_prg_banks}
{}

Mapper::~Mapper()
{}

Mapper_000::Mapper_000 (const u8 _prg_banks, const u8 _chr_banks)
: Mapper {_prg_banks, _chr_banks}
{}

Mapper_000::~Mapper_000 ()
{}

bool Mapper_000::cpu_read (const u16 address, u32& mapped_address, u8& data)
{
    /*
        16kb----------------------------
        program address      rom address
        $8000 - $BFFF   -->  $0000 - $3FFF
        $C000 - $FFFF   -->  $0000 - $3FFF

        32kb----------------------------
        program address      rom address
        $8000 - $FFFF   -->  $0000 - $7FFF

    */

    // address range check
    if (!(address >= 0x8000 && address <= 0xFFFF))
        return false;
    
    mapped_address = address & (prg_banks > 1 ? 0x7FFF : 0x3FFF);
    return true;
}

bool Mapper_000::cpu_write (u16 address, u32& mapped_address, const u8 data)
{
    return false;
}
