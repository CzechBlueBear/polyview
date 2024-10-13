#include "view.hpp"

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
