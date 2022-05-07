#ifndef GARDENJAR_H__CORE__WORKSPACE
#define GARDENJAR_H__CORE__WORKSPACE

#include <filesystem>

namespace gardenjar::core {

struct Workspace {
  Workspace(std::filesystem::path path) : path(path) { refresh(); }

  void refresh();

 protected:
  std::filesystem::path path;
};

}  // namespace gardenjar::core

#endif
