#include "view.hpp"

View::View(Document& document, sdl::Font& font, sdl::Size2d viewport_size_)
    : viewport_size(viewport_size_)
{
    line_count = document.size();
    document_size = calc_document_bounds(document, font);
    lines_visible = viewport_size.h / font.get_line_skip();
}

void View::render(sdl::Renderer& renderer, sdl::Font& font, Document& document, sdl::Color& bg_color, sdl::Color& fg_color)
{
    // spaces between basic elements, in pixels
    auto space_width = font.get_space_width();
    auto line_height = font.get_line_skip();

    // clear the viewport
    renderer.fill_rect(sdl::Rect(0, 0, viewport_size), bg_color);

    // how many lines we need to really draw
    auto lines_to_render = std::min(uint32_t(top_line_shown + lines_visible), line_count);

    // for each line...
    auto topleft = sdl::Point2d(scroll_x, 0);
    for (auto i = top_line_shown; i < lines_to_render; i++) {

        // render all pieces on the line (for long lines, some may not be visible)
        auto& line = document.get_line(i);
        for (auto& piece : line.pieces) {
            if (!piece.empty()) {
                auto piece_surface = font.render(piece.get_text(), sdl::Color::BLACK);
                auto piece_texture = renderer.texture_from_surface(piece_surface);
                renderer.put_texture(piece_texture, sdl::Rect(topleft, piece_texture.get_size()));
                topleft.x += piece_texture.get_size().w + space_width;
            }
        }

        // move to the new line
        topleft.x = -scroll_x;
        topleft.y += line_height;
    }
}

sdl::Size2d calc_document_bounds(Document& document, sdl::Font& font)
{
    auto space_width = font.get_space_width();

    auto document_bounds = sdl::Size2d(0, 0);
    for (auto i = 0u; i < document.size(); i++) {
        auto& line = document.get_line(i);

        // count the sizes of all pieces on the line
        auto line_bounds = sdl::Size2d(0, 0);
        for (auto& piece : line.pieces) {
            auto piece_size = font.calc_rendered_size(piece.get_text());
            line_bounds.w += piece_size.w;
            line_bounds.h = std::max(line_bounds.h, piece_size.h);
        }

        // add the spaces between pieces
        line_bounds.w += line.pieces.size() * space_width;

        document_bounds.w = std::max(document_bounds.w, line_bounds.w);
        document_bounds.h += line_bounds.h;
    }
    return document_bounds;
}
