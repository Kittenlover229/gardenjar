#ifndef GARDENJAR_H__WINDOWING__WINDOW_MANAGER
#define GARDENJAR_H__WINDOWING__WINDOW_MANAGER

#include <GLFW/glfw3.h>

#include <concepts>
#include <memory>

#include "Window.hpp"
#include "gardenjar/core/Workspace.hpp"

namespace gardenjar::windowing {

struct WindowManager {
  explicit WindowManager(core::Workspace&);
  ~WindowManager();

  void join();

  void add_window(std::shared_ptr<Window>&&);

  core::Workspace& ws;

 protected:
  GLFWwindow* glfw_window;
  std::vector<std::shared_ptr<Window>> windows;
};

}  // namespace gardenjar::windowing

#endif
