#include "GraphViewWindow.hpp"

#include <imgui.h>
#include <math.h>

using namespace gardenjar::windowing;

GraphViewWindow::GraphViewWindow(core::Workspace& ws)
    : ws(ws), Window(u8"Graph View") {}

void GraphViewWindow::draw_impl(WindowManager& wm) {
  auto draw_list = ImGui::GetWindowDrawList();
  draw_list->ChannelsSplit(2);
  auto window_pos = ImGui::GetWindowPos();

  // Draw links
  draw_list->ChannelsSetCurrent(0);
  for (const auto& link : ws.note_links) {
    auto [x1, y1] = coordinates[link.first];
    auto [x2, y2] = coordinates[link.second];

    draw_list->AddLine(
        ImVec2(x_offset + window_pos.x + x1, y_offset + window_pos.y + y1),
        ImVec2(x_offset + window_pos.x + x2, y_offset + window_pos.y + y2),
        IM_COL32(200, 200, 100, 255), 1.0f);
  }

  // Draw notes
  draw_list->ChannelsSetCurrent(1);
  for (const auto& note : ws.notes) {
    auto [x, y] = coordinates[note.id];
    draw_list->AddCircleFilled(
        ImVec2(x_offset + window_pos.x + x, y_offset + window_pos.y + y), 20,
        IM_COL32(200, 200, 100, 255));
  }

  draw_list->ChannelsMerge();

  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
      ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
    x_offset += ImGui::GetIO().MouseDelta.x;
    y_offset += ImGui::GetIO().MouseDelta.y;
  }
}
