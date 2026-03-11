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
    static constexpr std::string_view RENAME_FILE_POPUP_ID = "Rename file";
    static constexpr std::string_view DELETE_FILE_POPUP_ID = "Delete file";
    bool rename_popup_open_ = false;
    bool delete_popup_open_ = false;
    void open_file_default_editor();
    void rename_file_popup();
    void delete_file_popup();
    bool rename_file(const fs::path& old_path, const fs::path& new_path);
    bool delete_file(const fs::path& path);
};

} // namespace UI
