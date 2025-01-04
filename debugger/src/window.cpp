#include "window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>
#include <iostream>
#include <source_location>


namespace
{
    inline void print_error(const std::source_location& source = std::source_location::current())
    {
        std::clog << "file: "
                << source.file_name() << '('
                << source.line() << ':'
                << source.column() << ") `"
                << source.function_name() << "`: "
                << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


UI::Window::Window(const char* _title, int _width, int _height)
: title{_title}
, width{_width}
, height{_height}
{
    const auto result = SDL_Init(SDL_INIT_EVERYTHING);
    if (result < 0) print_error();
        
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (!window) print_error();

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if(!renderer) print_error();

    running = true;
}

UI::Window::~Window() 
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void UI::Window::poll(std::function<void(SDL_Event&)> callback) 
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
        callback(event);

        if (event.type == SDL_QUIT)
            running = false;
    }
}

bool UI::Window::is_running() const
{
    return running;
}


void UI::Window::update() const
{
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    SDL_RenderClear(renderer);
}

