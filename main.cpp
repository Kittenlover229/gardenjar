#include <iostream>

#include "gardenjar/core/Workspace.hpp"
#include "gardenjar/windowing/WindowManager.hpp"

using namespace gardenjar;

int main(void) {
  core::Workspace workspace(std::filesystem::current_path());
  auto wm = windowing::WindowManager(workspace);

  wm.join();

  return 0;
}
