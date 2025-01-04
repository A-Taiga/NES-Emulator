

#include "rom.h"
#include "MOS6502.h"
#include "window.h"

int main()
{
    // CPU::MOS6502 c(nullptr, nullptr);

    // c.print_instruction_set ();
    // NES_ROM rom ("../Super_mario_brothers.nes");
    // NES_ROM rom ("../zelda.nes");
    // NES_ROM rom("../Tetris.nes");
    // NES_ROM rom("../contra.nes");

    // auto read = [&rom](std::uint16_t address, std::uint8_t& data)
    // {
    //     return rom.cpu_read(address, data);
    // };

    // auto read = [&rom](auto address)
    // {
    //     return rom.get_prg_memory()[address];
    // };

    // CPU::MOS6502 cpu(read, nullptr);
    // cpu.decompile(rom.size());

    UI::Window window ("Debugger", 1000,1000);

    while (window.is_running())
    {
        window.poll([&](auto& event){});
        window.update();
    }


    return 0;
}