#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "MOS6502.h"
#include "window.h"
#include <cstdint>

namespace CPU {class MOS6502;}

namespace Debugger
{
    struct NES_Data
    {
        NES_Data (std::vector<std::uint8_t>& prg, std::vector<std::uint8_t>& chr, CPU::MOS6502& _cpu)
        : prg_memory {prg}
        , chr_memory {chr}
        , cpu {_cpu}
        {}

        NES_Data (NES_Data& other)
        : prg_memory {other.prg_memory}
        , chr_memory {other.chr_memory}
        , cpu {other.cpu}
        {}

        std::vector<std::uint8_t>& prg_memory;
        std::vector<std::uint8_t>& chr_memory;
        CPU::MOS6502& cpu;
    };

    class GUI
    {
    public:
        GUI (const char* title, const int width, const int height, NES_Data data);

        void run ();

    private:
        Window window;
        NES_Data data;
    };

}

#endif