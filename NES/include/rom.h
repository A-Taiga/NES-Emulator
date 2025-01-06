#ifndef ROM_H
#define ROM_H

#include <cstring>
#include <vector>
#include <memory>
#include "mapper.h"


class NES_ROM
{


public:

    NES_ROM(const char* file_name);

    std::uint32_t size() {return prg_memory.size();}

    bool cpu_read (u16 address, u8& data);

    u8 get_prg_bank_n () const;
    u8 get_chr_bank_n () const;

    std::vector<u8>& get_prg_memory ();
    std::vector<u8>& get_chr_memory ();


private:

    u8 prg_bank_n;
    u8 chr_bank_n;
    u8 mapper_id;

    std::shared_ptr <Mapper> mapper;

    int mirror;

    std::vector<u8> prg_memory;
    std::vector<u8> chr_memory;

};



#endif