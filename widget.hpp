#pragma once
#include "sdl_wrapper.hpp"
#include "settings.hpp"
#include <functional>
#include <memory>

/// Describes how a widget should be (re)sized.
class WidgetSizingInfo {
public:
    uint32_t min_width;     ///< Minimum width for the widget to be usable.
    uint32_t min_height;    ///< Minimum height for the widget to be usable.
    bool grow_x;        ///< Is it useful to grow this widget in the X direction?
    bool grow_y;        ///< Is it useful to grow this widget in the Y direction?
};

class Widget {
protected:
    sdl::Rect m_rect;
public:
    void set_rect(sdl::Rect rect) { m_rect = rect; }
    sdl::Rect get_rect() { return m_rect; }
    bool is_point_inside(sdl::Point2d point) { return m_rect.is_point_inside(point); }
    virtual void render(sdl::Renderer& renderer, Settings& settings) = 0;
    virtual WidgetSizingInfo get_sizing_info() = 0;
};

class VScrollbar : public Widget {
protected:
    uint32_t full_range = 0u;
    uint32_t marked_range_length = 0u;
    uint32_t marked_range_start = 0u;
    std::function<void(uint32_t)> value_callback;
public:
    const uint32_t MIN_INDICATOR_HEIGHT = 8u;

    VScrollbar() {}
    void render(sdl::Renderer& renderer, Settings& settings) override;

    WidgetSizingInfo get_sizing_info() override {
        return WidgetSizingInfo {
            .min_width = 32u,
            .min_height = 128u,
            .grow_x = false,
            .grow_y = true
        };
    }

    void set_full_range(uint32_t l) { full_range = l; }
    void set_marked_range(uint32_t start, uint32_t length) {
        marked_range_start = start;
        marked_range_length = length;
    }
    //void place_to_right_edge(sdl::Renderer& renderer);
    void set_value_callback(std::function<void(uint32_t)> cb) { value_callback = cb; }
};

class Label : public Widget {
protected:
    std::shared_ptr<sdl::Font> m_font;
    std::string m_text;
public:
    Label(std::shared_ptr<sdl::Font> font, std::string text) : m_font(font), m_text(text) {}
    void render(sdl::Renderer& renderer, Settings& settings) override;
    void set_text(std::string text) { m_text = text; }

    WidgetSizingInfo get_sizing_info() override {
        return WidgetSizingInfo {
            .min_width = 128u,
            .min_height = 32u,
            .grow_x = false,
            .grow_y = true
        };
    }
};
