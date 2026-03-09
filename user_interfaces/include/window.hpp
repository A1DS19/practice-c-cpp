#pragma once

#include <filesystem>
#include <string_view>

namespace UI {
namespace fs = std::filesystem;
class Window {
public:
    std::string_view label_;

    Window(std::string_view label) : label_(label) { current_directory_ = fs::current_path(); }
    void draw();
    void draw_menu();
    void draw_content();
    void draw_actions();
    void draw_filters();

private:
    fs::path current_directory_;
    fs::path selected_entry_;
};

} // namespace UI
