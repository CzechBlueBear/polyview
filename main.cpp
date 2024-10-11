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

std::array<char const*, 2> DEFAULT_FONT_PATHS = {
    "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf"
};

class Settings {
public:
    uint32_t font_size = 32u;
    SDL_Color background_color = SDL_Color{ 192, 192, 192, 255 };
    SDL_Color text_color = SDL_Color{ 0, 0, 0, 255 };
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

    Document document;
    document.load(file_name);
    std::cout << "loaded file: " << file_name << " (" << document.size() << " lines)\n";

    const std::string FONT_NAME = "/usr/share/fonts/liberation/LiberationMono-Regular.ttf";
    sdl::Font font(FONT_NAME, settings.font_size);

    auto window = std::make_unique<sdl::Window>("Viewer - " + file_name, sdl::Size2d(1024, 1024));
    auto renderer = std::make_unique<sdl::Renderer>(*window);

    auto top_visible_line = 0u;
    auto left_skip = 0u;
    auto lines_visible = renderer->get_output_size().h / font.get_line_skip();
    auto max_line_width = 0u;
    const auto HORIZONTAL_SCROLL_AMOUNT = 128;

    auto on_redraw = [&] {
        auto viewport_width = renderer->get_output_size().w;
        max_line_width = 0u;
        auto topleft = sdl::Point2d(-left_skip, 0);

        renderer->fill_rect(sdl::Rect(0, 0, renderer->get_output_size()), settings.background_color);

        for (auto i = top_visible_line; i < std::min(size_t(top_visible_line + lines_visible), document.size()); i++) {

            // render all pieces on the line
            auto line = document.get_line(i);
            for (auto piece : line.pieces) {
                if (!piece.empty()) {
                    auto line_texture = renderer->texture_from_surface(*font.render(piece.text, settings.text_color));
                    max_line_width = std::max(max_line_width, line_texture->get_size().w);
                    renderer->put_texture(*line_texture, sdl::Rect(topleft, line_texture->get_size()));
                    topleft.x += line_texture->get_size().w;
                    topleft.x += font.get_space_width();
                }
            }

            // new line
            topleft.x = -left_skip;
            topleft.y += font.get_line_skip();
        }

        renderer->present();
    };

    const uint64_t INTER_FRAME_PERIOD = 16;

    // the event loop
    bool exit_requested = false;
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
                    if (top_visible_line + lines_visible < document.size()) {
                        top_visible_line++;
                    }
                }
                else if (event.key.keysym.sym == SDLK_UP) {
                    if (top_visible_line > 0) {
                        top_visible_line--;
                    }
                }
                else if (event.key.keysym.sym == SDLK_LEFT) {
                    if (left_skip > 0) {
                        left_skip -= HORIZONTAL_SCROLL_AMOUNT;
                    }
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    if (left_skip < 4096) {
                        left_skip += HORIZONTAL_SCROLL_AMOUNT;
                    }
                }
            }
        }

        if (exit_requested) { break; }

        // draw frame
        uint64_t now = sdl::get_ticks();
        if (now > next_frame_time) {
            on_redraw();
            next_frame_time = sdl::get_ticks() + INTER_FRAME_PERIOD;
        }
        else {
            SDL_Delay(next_frame_time - now);
        }
    }
}
