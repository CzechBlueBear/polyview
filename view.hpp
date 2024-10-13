#pragma once

#include "sdl_wrapper.hpp"
#include "document.hpp"

class View {
public:
    uint32_t top_visible_line = 0u;
    uint32_t scroll_x = 0u;
};

sdl::Size2d calc_document_bounds(Document& document, sdl::Font& font);
