#ifndef WINDOW_H
#define WINDOW_H

#include <functional>
#include <string>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
struct SDL_Color;
union SDL_Event;

namespace UI
{
    class Window 
    {
    public:

        Window(const char* _title, int _width, int _height);
        ~Window();

        void poll(std::function<void(SDL_Event&)> callback);
        bool is_running() const;
        void update() const;

    private:

        SDL_Window* window;
        SDL_Renderer* renderer;

        std::string title;

        int width;
        int height;

        bool running;
    };
}

#endif