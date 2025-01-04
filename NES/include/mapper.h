#ifndef MAPPER_H
#define MAPPER_H


#include "utility.h"

class Mapper
{
public:

    Mapper(const u8 _prg_banks, const u8 _chr_banks);
    ~Mapper();

    virtual bool cpu_read  (const u16 address, u32& mapped_address, u8& data) = 0;
    virtual bool cpu_write (const u16 address, u32& mapped_address, const u8 data = 0) = 0;

protected:
    
    u8 prg_banks;
    u8 chr_banks;
    
};


class Mapper_000: public Mapper
{
public:

    Mapper_000(u8 _prg_banks, u8 _chr_banks);
    ~Mapper_000();

    bool cpu_read  (const u16 address, u32& mapped_address, u8& data) override;
    bool cpu_write (const u16 address, u32& mapped_address, const u8 data = 0) override;

private:

};

#endif