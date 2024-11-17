#include "widget.hpp"
#include "settings.hpp"

void VScrollbar::render(sdl::Renderer& renderer, Settings& settings)
{
    // draw the bar
    renderer.fill_rect(m_rect, settings.widget_background_color);
    uint32_t indicator_size = std::max(MIN_INDICATOR_HEIGHT, m_rect.h * marked_range_length / full_range);
    uint32_t indicator_position = m_rect.h * marked_range_start / full_range;

    // draw the indicator
    renderer.fill_rect(
        sdl::Rect(m_rect.x, indicator_position, m_rect.w, indicator_size),
        settings.widget_indicator_color);
}

/*
void VScrollbar::place_to_right_edge(sdl::Renderer& renderer)
{
    auto output_size = renderer.get_output_size();
    auto preferred_width = 32u;
    set_rect(sdl::Rect(output_size.w - preferred_width, 0, preferred_width, output_size.h));
}
*/

// ---- Label ----------------------------------------------------------------

void Label::render(sdl::Renderer& renderer, Settings& settings)
{
    auto surface = m_font->render(m_text, settings.widget_text_color);
    auto texture = renderer.surface_to_texture(surface);
    auto texture_size = texture.get_size();
    auto topleft = sdl::Point2d(
        (m_rect.w - texture_size.w)/2,
        (m_rect.h - texture_size.h)/2
    );
    renderer.put_texture(texture, topleft);
}
