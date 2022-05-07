#include <iostream>

#include "gardenjar/core/Workspace.hpp"
#include "gardenjar/windowing/GraphViewWindow.hpp"
#include "gardenjar/windowing/WindowManager.hpp"

using namespace gardenjar;

int main(void) {
  core::Workspace workspace(std::filesystem::current_path());
  auto wm = windowing::WindowManager(workspace);

  wm.add_window(std::make_shared<windowing::GraphViewWindow>(workspace));
  wm.join();

  return 0;
}
