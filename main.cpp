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

    Document document;
    document.load(file_name);
    std::cout << "loaded file: " << file_name << " (" << document.size() << " lines)\n";

    auto window = std::make_unique<sdl::Window>("Viewer - " + file_name, sdl::Size2d(1024, 1024));
    auto renderer = std::make_unique<sdl::Renderer>(*window);

    View view;

    auto lines_visible = renderer->get_output_size().h / font->get_line_skip();

    const auto HORIZONTAL_SCROLL_AMOUNT = 128;
    auto space_width = font->get_space_width();

    // size, in pixels, of the rendered document, if it would wholly fit
    auto document_bounds = calc_document_bounds(document, *font);
    std::cout << "document bounds: " << document_bounds.w << "x" << document_bounds.h << "\n";

    auto viewport_size = renderer->get_output_size();

    auto on_redraw = [&] {
        auto topleft = sdl::Point2d(-view.scroll_x, 0);

        renderer->fill_rect(sdl::Rect(0, 0, viewport_size), settings.background_color);

        auto lines_to_render = std::min(size_t(view.top_visible_line + lines_visible), document.size());

        // for each line...
        for (auto i = view.top_visible_line; i < lines_to_render; i++) {

            // render all pieces on the line
            auto& line = document.get_line(i);
            for (auto& piece : line.pieces) {
                if (!piece.empty()) {
                    auto piece_surface = font->render(piece.get_text(), sdl::Color::BLACK);
                    auto piece_texture = renderer->texture_from_surface(piece_surface);
                    renderer->put_texture(piece_texture, sdl::Rect(topleft, piece_texture.get_size()));
                    topleft.x += piece_texture.get_size().w + space_width;
                }
            }

            // new line
            topleft.x = -view.scroll_x;
            topleft.y += font->get_line_skip();
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
                    if (view.top_visible_line + lines_visible < document.size()) {
                        view.top_visible_line++;
                    }
                }
                else if (event.key.keysym.sym == SDLK_UP) {
                    if (view.top_visible_line > 0) {
                        view.top_visible_line--;
                    }
                }
                else if (event.key.keysym.sym == SDLK_LEFT) {
                    if (view.scroll_x > 0) {
                        view.scroll_x -= HORIZONTAL_SCROLL_AMOUNT;
                    }
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    if (view.scroll_x < document_bounds.w - viewport_size.w) {
                        view.scroll_x += HORIZONTAL_SCROLL_AMOUNT;
                    }
                }
                else if (event.key.keysym.sym == SDLK_HOME) {
                    view.scroll_x = 0;
                }
            }
            else if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y < 0) {
                    if (view.top_visible_line + lines_visible < document.size()) {
                        view.top_visible_line++;
                    }
                }
                else if (event.wheel.y > 0) {
                    if (view.top_visible_line > 0) {
                        view.top_visible_line--;
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
