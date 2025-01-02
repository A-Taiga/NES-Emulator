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

    bool cpu_read (std::uint16_t address, std::uint8_t& data);

    const std::vector<std::uint8_t>& get_prg_memory() const;
    const std::vector<std::uint8_t>& get_chr_memory() const;


private:

    std::uint8_t prg_bank_n;
    std::uint8_t chr_bank_n;
    std::uint8_t mapper_id;

    std::shared_ptr <Mapper> mapper;

    int mirror;

    std::vector<std::uint8_t> prg_memory;
    std::vector<std::uint8_t> chr_memory;

};



#endif