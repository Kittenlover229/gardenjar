#include <iostream>

#include "gardenjar/core/Workspace.hpp"

using namespace gardenjar;

int main(void) {
  core::Workspace workspace(std::filesystem::current_path());
  return 0;
}
