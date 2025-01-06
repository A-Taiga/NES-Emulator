
#include "debugger.h"
#include "rom.h"
#include "MOS6502.h"

int main()
{
    NES_ROM rom{"/home/anthony/Workspace/cpp/6502/roms/Super_mario_brothers.nes"};

    CPU::MOS6502 cpu {nullptr, nullptr};

    Debugger::NES_Data data {rom.get_prg_memory(), rom.get_chr_memory(), cpu};

    Debugger::GUI debugger {"Test", 1920, 1080, data};

    debugger.run ();

    return 0;
}