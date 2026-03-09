#include "window.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "main.hpp"

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <format>
#include <string_view>

using namespace UI;

void Window::draw() {
    constexpr static auto WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    constexpr static auto WINDOW_SIZE = ImVec2(WINDOW_W, WINDOW_H);
    constexpr static auto WINDOW_POS = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(WINDOW_SIZE);
    ImGui::SetNextWindowPos(WINDOW_POS);

    ImGui::Begin(label_.data(), nullptr, WINDOW_FLAGS);
    Window::draw_menu();
    ImGui::Separator();
    Window::draw_content();
    ImGui::Separator();
    Window::draw_actions();
    ImGui::Separator();
    Window::draw_filters();
    ImGui::End();
}

void Window::draw_menu() {
    if (ImGui::Button("Go up")) {
        if (current_directory_.has_parent_path()) {
            current_directory_ = current_directory_.parent_path();
        }
    }
    ImGui::SameLine();
    ImGui::Text("Current directory: %s", current_directory_.c_str());
}

void Window::draw_content() {
    for (const auto& entry : fs::directory_iterator(current_directory_)) {
        const auto IS_SELECTED = selected_entry_ == entry.path();
        const auto IS_DIRECTORY = entry.is_directory();
        const auto IS_FILE = entry.is_regular_file();
        const auto PATH = entry.path().filename().string();

        std::string icon;
        if (IS_FILE) {
            icon = std::format("[F] {}", PATH);
        } else if (IS_DIRECTORY) {
            icon = std::format("[D] {}", PATH);
        }

        if (ImGui::Selectable(icon.c_str(), IS_SELECTED)) {
            if (IS_DIRECTORY) {
                current_directory_ /= entry.path().filename();
            }

            selected_entry_ = entry.path();
        }
    }
}

void Window::draw_actions() {}

void Window::draw_filters() {
    static char extension_filter[16] = {"\0"};

    ImGui::Text("Filter by extension");
    ImGui::SameLine();
    // ### to hide label
    ImGui::InputText("###inFilter", extension_filter, sizeof(extension_filter));

    if (std::strlen(extension_filter) == 0) {
        return;
    }

    auto filtered_file_count = std::size_t{0};
    for (const auto& entry : fs::directory_iterator(current_directory_)) {
        if (!fs::is_regular_file(entry)) {
            continue;
        }

        auto filter_extension = entry.path().extension().string();
        std::erase(filter_extension, '.');

        if (entry.path().extension().string() == extension_filter ||
            filter_extension == extension_filter) {
            ++filtered_file_count;
        }
    }

    ImGui::Text("Number of files: %zu", filtered_file_count);
}
