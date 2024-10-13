#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_version.h>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <stdexcept>
#include <array>
#include <iostream>
#include "sdl_wrapper.hpp"
#include "document.hpp"
#include "view.hpp"

std::array<char const*, 2> DEFAULT_FONT_PATHS = {
    "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf"
};

class Settings {
public:
    uint32_t font_size = 32u;
    sdl::Color background_color = sdl::Color(192, 192, 192);
    sdl::Color text_color = sdl::Color(0, 0, 0);
    sdl::Size2d initial_window_size = sdl::Size2d(1024, 1280);
};

int main(int argc, char** argv)
{
    sdl::InitGuard guard;

    setlocale(LC_ALL,"");

    if (argc < 2) {
        std::cerr << "missing argument (file name)\n";
        return 1;
    }
    std::string file_name = argv[1];

    Settings settings;

    const std::string FONT_NAME = "/usr/share/fonts/liberation/LiberationMono-Regular.ttf";
    auto font = std::make_unique<sdl::Font>(FONT_NAME, settings.font_size);

    auto document = std::make_shared<Document>();
    document->load(file_name);
    std::cout << "loaded file: " << file_name << " (" << document->size() << " lines)\n";

    auto window = std::make_unique<sdl::Window>("Viewer - " + file_name, settings.initial_window_size);
    window->allow_resize();

    auto renderer = std::make_unique<sdl::Renderer>(*window);

    View view(document, *font, renderer->get_output_size());

    auto on_redraw = [&] {
        view.render(*renderer, *font, settings.background_color, settings.text_color);
        renderer->present();
    };

    const uint64_t INTER_FRAME_PERIOD = 16;

    // the event loop
    bool exit_requested = false;
    bool redraw_now = false;
    uint64_t next_frame_time = sdl::get_ticks();
    while (!exit_requested) {

        // handle all pending events
        sdl::Event event;
        while (sdl::poll_event(event)) {
            if (event.type == SDL_QUIT) {
                exit_requested = true;
                break;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    exit_requested = true;
                    break;
                }
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    view.scroll_line_down();
                    redraw_now = true;
                }
                else if (event.key.keysym.sym == SDLK_UP) {
                    view.scroll_line_up();
                    redraw_now = true;
                }
                else if (event.key.keysym.sym == SDLK_LEFT) {
                    view.scroll_block_left();
                    redraw_now = true;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    view.scroll_block_right();
                    redraw_now = true;
                }
                else if (event.key.keysym.sym == SDLK_HOME) {
                    view.scroll_x = 0;
                    redraw_now = true;
                }
            }
            else if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y < 0) {
                    view.scroll_line_down();
                    redraw_now = true;
                }
                else if (event.wheel.y > 0) {
                    view.scroll_line_up();
                    redraw_now = true;
                }
            }
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    sdl::Size2d new_window_size = sdl::Size2d(event.window.data1, event.window.data2);
                    view.update_viewport_size(*renderer, *font);
                    redraw_now = true;  // to my terrible surprise, this really improves visual response
                }
            }
        }

        if (exit_requested) { break; }

        // draw frame
        uint64_t now = sdl::get_ticks();
        if (redraw_now || now > next_frame_time) {
            on_redraw();
            next_frame_time = sdl::get_ticks() + INTER_FRAME_PERIOD;
        }
        else {
            SDL_Delay(next_frame_time - now);
        }
        redraw_now = false;
    }
}
