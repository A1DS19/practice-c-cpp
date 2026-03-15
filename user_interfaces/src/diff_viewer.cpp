#include "diff_viewer.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "main.hpp"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

using namespace DiffViewer;

void Window::draw() {
    constexpr static auto WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    constexpr static auto WINDOW_SIZE = ImVec2(WINDOW_W, WINDOW_H);
    constexpr static auto WINDOW_POS = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(WINDOW_SIZE);
    ImGui::SetNextWindowPos(WINDOW_POS);

    ImGui::Begin("Diff Viewer", nullptr, WINDOW_FLAGS);
    draw_selection();
    draw_diff_view();
    draw_stats();
    ImGui::End();
}

void Window::draw_selection() {
    if (ImGui::Button("Browse Left")) {
        file_diff_1_->file_browser_.Open();
    }
    file_diff_1_->file_browser_.Display();
    if (file_diff_1_->file_browser_.HasSelected()) {
        file_diff_1_->path_ = file_diff_1_->file_browser_.GetSelected().string();
        file_diff_1_->file_browser_.ClearSelected();
    }

    if (ImGui::Button("Browse Rigth")) {
        file_diff_2_->file_browser_.Open();
    }

    file_diff_2_->file_browser_.Display();
    if (file_diff_2_->file_browser_.HasSelected()) {
        file_diff_2_->path_ = file_diff_2_->file_browser_.GetSelected().string();
        file_diff_2_->file_browser_.ClearSelected();
    }

    if (ImGui::Button("Compare")) {
        load_file_content(file_diff_1_);
        load_file_content(file_diff_2_);
        create_diff();
    }
}

void Window::draw_diff_view() {
    const auto parent_size = ImVec2(ImGui::GetContentRegionAvail().x, 500.0F);
    const auto child_size = ImVec2((parent_size.x / 2.0F) - 40.0F, parent_size.y);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));

    ImGui::BeginChild("parent", parent_size, true);

    ImGui::BeginChild("diff_1", child_size, false);
    for (std::size_t i = 0; i < file_diff_1_->content_.size(); ++i) {
        if (!file_diff_1_->diff_result_[i].empty()) {
            ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "%s",
                               file_diff_1_->content_[i].data());
        } else {
            ImGui::Text("%s", file_diff_1_->content_[i].data());
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("diff_2");
    for (std::size_t i = 0; i < file_diff_2_->content_.size(); ++i) {
        ImGui::Text("%s", file_diff_2_->content_[i].data());
    }
    ImGui::EndChild();

    ImGui::EndChild();

    ImGui::PopStyleVar();
}

void Window::draw_stats() {
    auto diff_lines_count = std::size_t{0};

    for (const auto& line : file_diff_1_->diff_result_) {
        if (!line.empty()) {
            ++diff_lines_count;
        }
    }

    ImGui::Text("Diff lines count %lu", diff_lines_count);
}

void Window::create_diff() {
    file_diff_1_->diff_result_.clear();
    file_diff_2_->diff_result_.clear();

    const auto max_lines = std::max(file_diff_1_->content_.size(), file_diff_2_->content_.size());
    for (std::size_t i = 0; i < max_lines; ++i) {
        const auto line_1 = i < file_diff_1_->content_.size() ? file_diff_1_->content_[i] : "EMPTY";
        const auto line_2 = i < file_diff_2_->content_.size() ? file_diff_2_->content_[i] : "EMPTY";

        if (line_1 != line_2) {
            file_diff_1_->diff_result_.push_back(line_1);
            file_diff_2_->diff_result_.push_back(line_2);
        } else {
            file_diff_1_->diff_result_.push_back("");
            file_diff_2_->diff_result_.push_back("");
        }
    }
}

void Window::load_file_content(const Window::FileDiffPtr& file_diff) {
    auto in_file = std::ifstream(file_diff->path_.data());

    if (!in_file.is_open()) {
        return;
    }

    auto line = std::string{};
    while (std::getline(in_file, line)) {
        file_diff->content_.push_back(line);
    }

    in_file.close();
}

void Window::save_file_content(const Window::FileDiffPtr& file_diff) {
    auto out_file = std::ofstream(file_diff->path_.data());

    if (!out_file.is_open()) {
        return;
    }

    for (const auto& line : file_diff->content_) {
        out_file << line << "\n";
    }

    out_file.close();
}
