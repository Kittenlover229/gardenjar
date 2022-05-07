#ifndef GARDENJAR_H__WINDOWING__GRAPH_VIEW_WINDOW
#define GARDENJAR_H__WINDOWING__GRAPH_VIEW_WINDOW

#include <tuple>

#include "Window.hpp"
#include "gardenjar/core/Note.hpp"
#include "gardenjar/core/Workspace.hpp"

namespace gardenjar::windowing {

struct GraphViewWindow : Window {
  GraphViewWindow(core::Workspace&);

  virtual void draw_impl(WindowManager&) override;

 protected:
  core::Workspace& ws;
  std::map<core::NoteID, std::pair<int, int>> coordinates;
  int scale = 150;
  int x_offset = 150, y_offset = 150;
};

}  // namespace gardenjar::windowing

#endif
