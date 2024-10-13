#pragma once

#include "sdl_wrapper.hpp"
#include "document.hpp"

class View {
public:
    uint32_t top_line_shown = 0u;       ///< Top line shown in the view.
    uint32_t scroll_x = 0u;             ///< Current amount of scroll to the right (in pixels).
    uint32_t line_count = 0u;           ///< Number of lines in the document.
    uint32_t lines_visible = 0u;        ///< Max number of lines visible at once in the view.
    sdl::Size2d viewport_size;          ///< Size of the area used for view, in pixels.
    sdl::Size2d document_size;          ///< Document size in pixels.

    View(Document& document, sdl::Font& font, sdl::Size2d viewport_size_);
    void render(sdl::Renderer& renderer, sdl::Font& font, Document& document, sdl::Color& bg_color, sdl::Color& fg_color);
};

sdl::Size2d calc_document_bounds(Document& document, sdl::Font& font);
