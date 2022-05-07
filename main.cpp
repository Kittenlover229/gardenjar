#include <iostream>
#include <thread>

#include "gardenjar/core/Workspace.hpp"
#include "gardenjar/windowing/WindowManager.hpp"

using namespace gardenjar;

int main(void) {
  core::Workspace workspace(std::filesystem::current_path());
  auto wm = windowing::WindowManager(workspace);

  auto wm_thread = std::jthread([&]() {
    wm.add_window(
        std::make_shared<windowing::Window>(std::u8string(u8"Mr Window")));

    wm.join();
  });

  wm_thread.join();

  return 0;
}
