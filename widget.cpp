#include "widget.hpp"
#include "settings.hpp"

VScrollbar::VScrollbar()
{
}

void VScrollbar::render(sdl::Renderer& renderer, Settings& settings)
{
    auto output_size = renderer.get_output_size();

    // draw the bar
    renderer.fill_rect(m_rect, settings.widget_background_color);
    uint32_t indicator_size = std::max(MIN_INDICATOR_HEIGHT, m_rect.h * marked_range_length / full_range);
    uint32_t indicator_position = m_rect.h * marked_range_start / full_range;

    // draw the indicator
    renderer.fill_rect(
        sdl::Rect(m_rect.x, indicator_position, SCROLLBAR_WIDTH, indicator_size),
        settings.widget_indicator_color);
}

void VScrollbar::place_to_right_edge(sdl::Renderer& renderer)
{
    auto output_size = renderer.get_output_size();
    set_rect(sdl::Rect(output_size.w - SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, output_size.h));
}
