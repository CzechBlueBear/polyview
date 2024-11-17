#pragma once
#include "widget.hpp"
#include <memory>
#include <vector>

class Container : public virtual Widget {
public:
    virtual void layout() = 0;
};

/**
 * Lays out its elements horizontally. Width of each widget is derived
 * from the minimum width, and if extra room is available, it is divided
 * evenly between growable widgets.
 */
class HContainer : public virtual Container {
protected:
    std::vector<std::shared_ptr<Widget>> m_widgets;
public:
    void add(std::shared_ptr<Widget> widget) { m_widgets.push_back(widget); }
    void layout() override;
    void render(sdl::Renderer& renderer, Settings& settings) override;
    WidgetSizingInfo get_sizing_info() override;
};
