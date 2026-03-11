#include "window.hpp"

#include "imgui.h"
#include "main.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
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

void Window::draw_actions() {
    if (fs::is_directory(selected_entry_)) {
        ImGui::Text("Selected dir: %s", selected_entry_.string().c_str());
    } else if (fs::is_regular_file(selected_entry_)) {
        ImGui::Text("Selected file: %s", selected_entry_.string().c_str());
    } else {
        ImGui::Text("Nothing selected");
    }

    if (fs::is_regular_file(selected_entry_)) {
        if (ImGui::Button("Open file")) {
            open_file_default_editor();
        }
    }

    if (ImGui::Button("Rename")) {
        rename_popup_open_ = true;
        ImGui::OpenPopup(RENAME_FILE_POPUP_ID.data());
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete")) {
        delete_popup_open_ = true;
        ImGui::OpenPopup(DELETE_FILE_POPUP_ID.data());
    }

    rename_file_popup();
    delete_file_popup();
}

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

void Window::open_file_default_editor() {
#ifdef _WIN32
    const auto COMMAND = "start \"\" \"" + selected_entry_.string() + "\"";
#elif __APPLE__
    const auto COMMAND = "open \"" + selected_entry_.string() + "\"";
#else
    const auto COMMAND = "xdg-open \"" + selected_entry_.string() + "\"";
#endif

    std::system(COMMAND.c_str());
}

void Window::rename_file_popup() {
    if (ImGui::BeginPopupModal(RENAME_FILE_POPUP_ID.data(), &rename_popup_open_,
                               ImGuiWindowFlags_NoResize)) {
        static char file_name_buff[512] = "\0";
        ImGui::Text("New name: ");
        ImGui::InputText("###new_file_name", file_name_buff, sizeof(file_name_buff));

        if (ImGui::Button("Save")) {
            auto new_path = selected_entry_.parent_path() / file_name_buff;
            if (rename_file(selected_entry_, new_path)) {
                selected_entry_ = new_path;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

void Window::delete_file_popup() {
    if (ImGui::BeginPopupModal(DELETE_FILE_POPUP_ID.data(), &delete_popup_open_,
                               ImGuiWindowFlags_NoResize)) {

        ImGui::Text("Are you sure?");

        if (ImGui::Button("Yes")) {
            if (delete_file(selected_entry_)) {
                selected_entry_.clear();
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("No")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

bool Window::rename_file(const fs::path& old_path, const fs::path& new_path) {
    try {
        fs::rename(old_path, new_path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return false;
    }
}

bool Window::delete_file(const fs::path& path) {
    try {
        fs::remove(path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return false;
    }
}
