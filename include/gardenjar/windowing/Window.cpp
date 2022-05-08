#include "Window.hpp"

#include <imgui.h>

using namespace gardenjar::windowing;

Window::Window(std::u8string window_name)
    : _keep_alive(true), window_name(window_name) {}

Window::~Window() {}

void Window::draw_impl(WindowManager&) {
  ImGui::TextColored(ImVec4(1, 0, 0, 1), "No custom drawing logic defined.");
}

void Window::close() { _keep_alive = false; }

void Window::draw(WindowManager& wm) {
  bool wants_stay_open = true;
  ImGui::Begin((const char*)(window_name).c_str(), &wants_stay_open,
               ImGuiWindowFlags_NoFocusOnAppearing);
  if (wants_stay_open)
    draw_impl(wm);
  else
    this->close();
  ImGui::End();
}

bool Window::keep_alive() { return this->_keep_alive; }
