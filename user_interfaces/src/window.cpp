#include "window.hpp"

#include "imgui.h"
#include "main.hpp"

using namespace UI;

void Window::draw(std::string_view label) {
    constexpr static auto window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    constexpr static auto window_size = ImVec2(WINDOW_W, WINDOW_H);
    constexpr static auto window_pos = ImVec2(0.0F, 0.0F);

    ImGui::SetNextWindowSize(window_size);
    ImGui::SetNextWindowPos(window_pos);

    ImGui::Begin(label.data(), nullptr, window_flags);
    Window::draw_content();
    Window::draw_actions();
    Window::draw_filters();
    ImGui::End();
}

void Window::draw_menu() {}

void Window::draw_content() {}

void Window::draw_actions() {}

void Window::draw_filters() {}
