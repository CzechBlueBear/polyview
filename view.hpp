#pragma once

#include "sdl_wrapper.hpp"
#include "document.hpp"
#include "settings.hpp"
#include "widget.hpp"
#include <memory>

class View : public virtual Widget {
protected:
    std::shared_ptr<Document> m_document;
    std::shared_ptr<sdl::Font> m_font;
    VScrollbar m_scrollbar;
public:
    const uint32_t HORIZONTAL_SCROLL_AMOUNT = 128;
    const uint32_t SCROLLBAR_WIDTH = 32;
    const uint32_t MIN_INDICATOR_SIZE = 8;

    uint32_t top_line_shown = 0u;       ///< Top line shown in the view.
    uint32_t max_lines_shown = 0u;      ///< Max number of lines visible at once in the view.
    uint32_t scroll_x = 0u;             ///< Current amount of scroll to the right (in pixels).
    sdl::Size2d viewport_size;          ///< Size of the area used for view, in pixels.
    sdl::Size2d document_size;          ///< Document size in pixels.

    View(std::shared_ptr<Document> document, std::shared_ptr<sdl::Font> font, sdl::Size2d viewport_size_);
    void scroll_line_up();
    void scroll_line_down();
    void scroll_block_left();
    void scroll_block_right();
    void update_viewport_size(sdl::Renderer& renderer);
    void scroll_to_indicator(uint32_t new_indicator_position);
    VScrollbar& get_scrollbar() { return m_scrollbar; }

    void render(sdl::Renderer& renderer, Settings& settings) override;
    WidgetSizingInfo get_sizing_info() override {
        return WidgetSizingInfo {
            .min_width = 128u,
            .min_height = 128u,
            .grow_x = true,
            .grow_y = true
        };
    }
};

sdl::Size2d calc_document_bounds(Document& document, sdl::Font& font);
