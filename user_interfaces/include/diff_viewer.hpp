#pragma once

#include "imgui.h"

#include <format>
#include <imfilebrowser.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace DiffViewer {
class Window {
public:
    struct FileDiff {
        std::string_view tag_;
        ImGui::FileBrowser file_browser_;
        std::string path_;
        std::vector<std::string> content_;
        std::vector<std::string> diff_result_;

        FileDiff(std::string_view tag) : tag_{tag} {
            file_browser_.SetTitle(std::format("Browse content for {}", tag_));
            file_browser_.SetTypeFilters({".h", ".cpp", ".hpp"});
        }
    };
    using FileDiffPtr = std::unique_ptr<FileDiff>;

    Window() {
        file_diff_1_ = std::make_unique<Window::FileDiff>("Left");
        file_diff_2_ = std::make_unique<Window::FileDiff>("Rigth");
    }

    void draw();
    void draw_selection();
    void draw_diff_view();
    void draw_stats();
    void create_diff();
    void load_file_content(const FileDiffPtr& file_diff);
    void save_file_content(const FileDiffPtr& file_diff);

private:
    FileDiffPtr file_diff_1_;
    FileDiffPtr file_diff_2_;
};
} // namespace DiffViewer
