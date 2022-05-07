#include <iostream>

#include "gardenjar/core/Workspace.hpp"

using namespace gardenjar;

int main(void) {
  core::Workspace workspace(std::filesystem::current_path());

  std::cout << "Hello, world!" << std::endl;
  return 0;
}
