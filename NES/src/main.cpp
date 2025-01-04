
#include "debugger.h"


int main()
{
    Debugger::GUI debugger {"Test", 1920, 1080};

    debugger.run();

    return 0;
}