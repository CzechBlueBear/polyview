#pragma once

#include <cstdint>
#include "sdl_wrapper.hpp"

class Settings {
public:
    uint32_t font_size = 32u;
    sdl::Color background_color = sdl::Color(192, 192, 192);
    sdl::Color text_color       = sdl::Color(0, 0, 0);
    sdl::Color widget_background_color = sdl::Color(248, 255, 248);
    sdl::Color widget_indicator_color = sdl::Color(127, 127, 255);
    sdl::Size2d initial_window_size = sdl::Size2d(1024, 1280);
};
