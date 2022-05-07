#ifndef GARDENJAR_H__WINDOWING__WINDOW_MANAGER
#define GARDENJAR_H__WINDOWING__WINDOW_MANAGER

#include <GLFW/glfw3.h>

#include "gardenjar/core/Workspace.hpp"

namespace gardenjar::windowing {

struct WindowManager {
  explicit WindowManager(core::Workspace&);
  ~WindowManager();

  void join();

  core::Workspace& ws;
  GLFWwindow* glfw_window;
};

}  // namespace gardenjar::windowing

#endif
