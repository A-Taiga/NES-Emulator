#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "window.h"


struct ImGuiIO;

namespace Debugger
{
    class GUI
    {
    public:
        GUI (const char* title, const int width, const int height);


        void run ();

    private:
        Window window;
        ImGuiIO& io;
    };
}



#endif