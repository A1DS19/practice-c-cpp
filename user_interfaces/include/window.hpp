#pragma once

#include <string_view>

namespace UI {
class Window {
public:
    Window(std::string_view label) { draw(label); }
    void draw(std::string_view label);
    void draw_menu();
    void draw_content();
    void draw_actions();
    void draw_filters();
};

} // namespace UI
