#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "window.h"



namespace Debugger
{
    class GUI
    {
    public:
        GUI (const char* title, const int width, const int height);

        void run ();

    private:
        Window window;
    };
}



#endif