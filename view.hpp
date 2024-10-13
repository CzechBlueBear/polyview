#pragma once

#include "sdl_wrapper.hpp"
#include "document.hpp"

class View {
protected:
    std::shared_ptr<Document> m_document;
public:
    const uint32_t HORIZONTAL_SCROLL_AMOUNT = 128;

    uint32_t top_line_shown = 0u;       ///< Top line shown in the view.
    uint32_t scroll_x = 0u;             ///< Current amount of scroll to the right (in pixels).
    uint32_t line_count = 0u;           ///< Number of lines in the document.
    uint32_t max_lines_visible = 0u;    ///< Max number of lines visible at once in the view.
    sdl::Size2d viewport_size;          ///< Size of the area used for view, in pixels.
    sdl::Size2d document_size;          ///< Document size in pixels.

    View(std::shared_ptr<Document> document, sdl::Font& font, sdl::Size2d viewport_size_);
    void render(sdl::Renderer& renderer, sdl::Font& font, sdl::Color& bg_color, sdl::Color& fg_color);
    void scroll_line_up();
    void scroll_line_down();
    void scroll_block_left();
    void scroll_block_right();
    void update_viewport_size(sdl::Renderer& renderer, sdl::Font& font);
};

sdl::Size2d calc_document_bounds(Document& document, sdl::Font& font);
