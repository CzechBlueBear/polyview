#include "container.hpp"
#include "widget.hpp"

void HContainer::layout()
{
    uint32_t total_min_width = 0, cap_height = 0;
    for (auto widget : m_widgets) {
        auto sizing_info = widget->get_sizing_info();
        total_min_width += sizing_info.min_width;
        cap_height = std::max(cap_height, sizing_info.min_height);
    }

    sdl::Point2d topleft;
    for (auto widget : m_widgets) {
        auto sizing_rules = widget->get_sizing_info();
        widget->set_rect(sdl::Rect(topleft, sdl::Size2d(sizing_rules.min_width, cap_height)));
        topleft.x += sizing_rules.min_width;
    }
}
