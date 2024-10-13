#pragma once
#include "sdl_wrapper.hpp"
#include "settings.hpp"

class Widget {
protected:
    sdl::Rect m_rect;
public:
    void set_rect(sdl::Rect rect) { m_rect = rect; }
    sdl::Rect get_rect() { return m_rect; }
    virtual void render(sdl::Renderer& renderer, Settings& settings) = 0;
};

class VScrollbar : public Widget {
protected:
    uint32_t full_range = 0u;
    uint32_t marked_range_length = 0u;
    uint32_t marked_range_start = 0u;
public:
    const uint32_t SCROLLBAR_WIDTH = 32u;
    const uint32_t MIN_INDICATOR_HEIGHT = 8u;

    VScrollbar();
    void render(sdl::Renderer& renderer, Settings& settings);
    void set_full_range(uint32_t l) { full_range = l; }
    void set_marked_range(uint32_t start, uint32_t length) {
        marked_range_start = start;
        marked_range_length = length;
    }
    void place_to_right_edge(sdl::Renderer& renderer);
};
