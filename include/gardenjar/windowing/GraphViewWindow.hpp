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
  void shape_graph();

 protected:
  core::Workspace& ws;

  bool show_controls;
  std::map<core::NoteID, std::pair<float, float>> coordinates;
  float ideal_length;
  float c_rep;
  float c_spring;
  float damping;

  int scale = 150;
  float x_offset = 150, y_offset = 150;
};

}  // namespace gardenjar::windowing

#endif
