#include "debugger.h"
#include "MOS6502.h"
#include "hex_editor.h"
#include <cstring>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "mos6502_instructions.h"
#include <SDL.h>
#include <cstdint>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


namespace
{
    void print_instruction_set (const CPU::MOS6502& cpu)
    {

        const auto& padding_x = ImGui::GetStyle().FramePadding.x * 2;
        const auto& padding_y = ImGui::GetStyle().FramePadding.y * 2;

        ImGui::Begin ("Instruction Set");
        
        for (auto i = 0; i < 256; ++i)
        {

            const auto& ins = cpu.get_instruction(i);

            if (i % 16 != 0)
                ImGui::SameLine();
            
            if (ins.instruction == CPU::_6502::Opcode::XXX)
            {
                const auto size = ImGui::CalcTextSize(ins.mnemonic);
                ImGui::Dummy({size.x + padding_x, size.y + padding_y});
                continue;
            }
            ImGui::PushID(i);
            ImGui::Button(ins.mnemonic);
            if(ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
            {
                ImGui::BeginTooltip();
                ImGui::Text("%s", ins.instruction_info);
                ImGui::Text("%s", ins.mode_info);
                ImGui::EndTooltip();
            }
            ImGui::PopID();
        }

        ImGui::End();
    }

    [[maybe_unused]]
    void ins_info_button (u8 instruction, u16 index, const CPU::MOS6502& cpu) 
    {
        const auto& ins = cpu.get_instruction(instruction);
        ImGui::SameLine(ImGui::CalcTextSize("F").x * 27);
        ImGui::PushID(index);
        ImGui::Button(ins.mnemonic);
        if(ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
        {
            ImGui::BeginTooltip();
            ImGui::Text("%s", ins.instruction_info);
            ImGui::Text("%s", ins.mode_info);
            ImGui::EndTooltip();
        }
        ImGui::PopID();
    }

    void decompiler (const Debugger::NES_Data& data)
    {
        std::uint16_t index = 0;
        const auto read = [&data](auto index){ return data.prg_memory[index]; };
        ImGui::Begin ("Code");
        
        while (index < data.prg_memory.size())
        {
            const auto& ins_info = data.cpu.get_instruction(data.prg_memory[index]);
            
            switch (ins_info.mode)
            {
                case CPU::_6502::Mode::ABS:
                    ImGui::Text ("%04X: %02X %02X %02X %s $%04X", data.address_offset + index, read(index), read(index+1), read(index+2), ins_info.mnemonic, (read(index+2) << 8) | read(index+1));
                    ins_info_button (read(index), index, data.cpu);
                    index += 3;
                break;
                case CPU::_6502::Mode::ABX:
                    ImGui::Text ("%04X: %02X %02X %02X %s $%04X, X", data.address_offset + index, read(index), read(index+1), read(index+2), ins_info.mnemonic, (read(index+2) << 8) | read(index+1));
                    ins_info_button (read(index), index, data.cpu);
                    index += 3;
                break;
                case CPU::_6502::Mode::ABY:
                    ImGui::Text ("%04X: %02X %02X %02X %s", data.address_offset + index, read(index), read(index+1), read(index+2), ins_info.mnemonic);
                    ins_info_button (read(index), index, data.cpu);
                    index += 3;
                break;
                case CPU::_6502::Mode::IMM:
                    ImGui::Text ("%04X: %02X %02X %6s #$%02X", data.address_offset + index, read(index), read(index+1), ins_info.mnemonic, read(index+1));
                    ins_info_button (read(index), index, data.cpu);
                    index += 2;
                break;
                case CPU::_6502::Mode::ACC:
                case CPU::_6502::Mode::IMP:
                    ImGui::Text ("%04X: %02X %9s",data.address_offset +  index, read(index), ins_info.mnemonic);
                    ins_info_button (read(index), index, data.cpu);
                    index += 1;
                break;
                case CPU::_6502::Mode::IND:
                    ImGui::Text ("%04X: %02X %02X %02X %s", data.address_offset + index, read(index), read(index+1), read(index+2), ins_info.mnemonic);
                    ins_info_button (read(index), index, data.cpu);
                    index += 3;
                break;
                case CPU::_6502::Mode::XIZ:
                    ImGui::Text ("%04X: %02X %02X %6s $%02X", data.address_offset + index, read(index), read(index+1), ins_info.mnemonic, read(index+1));
                    ins_info_button (read(index), index, data.cpu);
                    index+= 2;
                break;
                case CPU::_6502::Mode::YIZ:
                    ImGui::Text ("%04X: %02X %02X %6s $%02X", data.address_offset + index, read(index), read(index+1), ins_info.mnemonic, read(index+1));
                    ins_info_button (read(index), index, data.cpu);
                    index += 2;
                break;
                case CPU::_6502::Mode::REL:
                    if (std::strcmp(ins_info.mnemonic, "CPX"))
                    {
                        std::uint16_t result = read (index+1);
                        if (result & 0x80) result |= 0xFF00;
                        std::uint16_t addr = index+2 + result;
                        ImGui::Text ("%04X: %02X %02X %6s $%04X", data.address_offset + index, read(index), read(index+1), ins_info.mnemonic, addr); 
                        ins_info_button (read(index), index, data.cpu);
                        index += 2;
                    }
                break;
                case CPU::_6502::Mode::ZPG:
                    ImGui::Text ("%04X: %02X %02X %6s %02X", data.address_offset + index, read(index), read(index+1), ins_info.mnemonic, read(index+1));
                    ins_info_button (read(index), index, data.cpu);
                    index += 2;
                break;
                case CPU::_6502::Mode::ZPX:
                    ImGui::Text ("%04X: %02X %02X %6s", data.address_offset + index, read(index), read(index+1), ins_info.mnemonic);
                    ins_info_button (read(index), index, data.cpu);
                    index += 2;
                break;
                case CPU::_6502::Mode::ZPY:
                    ImGui::Text ("%04X: %02X %02X %6s", data.address_offset + index, read(index), read(index+1), ins_info.mnemonic);
                    ins_info_button (read(index), index, data.cpu);
                    index += 2;
                break;
            }
        }

        ImGui::End();
    }
}

Debugger::GUI::GUI (const char* title, const int width, const int height, NES_Data data)
: window {title, width, height}
, data {data}
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
}

void Debugger::GUI::run ()
{
    auto& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    io.Fonts->AddFontFromFileTTF("/home/anthony/Workspace/cpp/6502/debugger/imgui/misc/fonts/Cousine-Regular.ttf", 25);


    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window.get_window (), window.get_gl_context ());
    ImGui_ImplOpenGL3_Init(window.get_glsl_version ().c_str());

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    Hex_Editor prg_memory {"prg memory", data.prg_memory.size(), 0, data.prg_memory.size(), sizeof(std::uint8_t), data.prg_memory.data()};
    Hex_Editor chr_memory {"chr memory", data.chr_memory.size(), 0, data.chr_memory.size(), sizeof(std::uint8_t), data.chr_memory.data()};

    while (window.is_running ())
    {
        window.poll ([&] (auto& event) {
            ImGui_ImplSDL2_ProcessEvent  (&event);
        });

        if (SDL_GetWindowFlags(window.get_window ()) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay (10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();
        
        prg_memory.present();
        chr_memory.present();

        ImGui::Begin("Test");
        const std::uint8_t SR = data.cpu.get_SR();
        ImGui::Text("C: %d", static_cast <int> (SR >> 0) & 1);
        ImGui::Text("Z: %d", static_cast <int> (SR >> 1) & 1);
        ImGui::Text("I: %d", static_cast <int> (SR >> 2) & 1);
        ImGui::Text("D: %d", static_cast <int> (SR >> 3) & 1);
        ImGui::Text("B: %d", static_cast <int> (SR >> 4) & 1);
        ImGui::Text("_: %d", static_cast <int> (SR >> 5) & 1);
        ImGui::Text("V: %d", static_cast <int> (SR >> 6) & 1);
        ImGui::Text("N: %d", static_cast <int> (SR >> 7) & 1);
        ImGui::End();
        print_instruction_set(data.cpu);

        decompiler(data);

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window.get_window ());

    }
}
