#include "GraphViewWindow.hpp"

#include <imgui.h>
#include <math.h>

using namespace gardenjar::windowing;

GraphViewWindow::GraphViewWindow(core::Workspace& ws)
    : ws(ws),
      Window(u8"Graph View"),
      c_rep(2),
      ideal_length(100),
      c_spring(1),
      damping(1),
      show_controls(false) {}

void GraphViewWindow::draw_impl(WindowManager& wm) {
  auto draw_list = ImGui::GetWindowDrawList();
  draw_list->ChannelsSplit(2);
  auto window_pos = ImGui::GetWindowPos();

  if (ImGui::BeginMenu("Options")) {
    if (ImGui::MenuItem("Show Controls")) {
      show_controls = !show_controls;
    }
    ImGui::EndMenu();
  }

  if (show_controls) {
    ImGui::Begin("Controls");
    if (ImGui::Button("Reset")) {
      srand(time(NULL));
      coordinates.clear();
    }
    ImGui::SliderFloat("Ideal Length", &ideal_length, 1, 300);
    ImGui::SliderFloat("Damping", &damping, 0, 1);
    ImGui::SliderFloat("Repel", &c_rep, 1, 9);
    ImGui::SliderFloat("Spring", &c_spring, 1, 9);
    ImGui::End();
  }

  shape_graph();

  // Draw links
  draw_list->ChannelsSetCurrent(0);

  for (const auto& [note_id, other_note_ids] : ws.links) {
    auto [x1, y1] = coordinates[note_id];
    for (const auto& other_note_id : other_note_ids) {
      auto [x2, y2] = coordinates[other_note_id];
      draw_list->AddLine(
          ImVec2(x_offset + window_pos.x + x1, y_offset + window_pos.y + y1),
          ImVec2(x_offset + window_pos.x + x2, y_offset + window_pos.y + y2),
          IM_COL32(200, 200, 100, 255), 1.0f);
    }
  }

  // Draw notes
  draw_list->ChannelsSetCurrent(1);
  for (const auto& note : ws.notes) {
    auto [x, y] = coordinates[note.id];
    draw_list->AddCircleFilled(
        ImVec2(x_offset + window_pos.x + x, y_offset + window_pos.y + y), 5,
        IM_COL32(200, 200, 100, 255));
  }

  draw_list->ChannelsMerge();

  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
      ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
    x_offset += ImGui::GetIO().MouseDelta.x;
    y_offset += ImGui::GetIO().MouseDelta.y;
  }
}

void GraphViewWindow::shape_graph() {
  if (ws.notes.size() != coordinates.size())
    for (const auto& note : ws.notes) {
      coordinates[note.id].first = ((float)(rand() % 150) / 150) * 400;
      coordinates[note.id].second = ((float)(rand() % 150) / 150) * 400;
    }

  // Using Eades Force-Drived algorithm
  std::map<core::NoteID, std::pair<float, float>> forces;
  for (const auto& note : ws.notes) {
    float fx = 0, fy = 0;
    auto [x1, y1] = coordinates[note.id];

    for (const auto& other_note : ws.notes) {
      if (other_note.id == note.id) continue;

      auto [x2, y2] = coordinates[other_note.id];
      auto d = std::sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
      if (d == 0) d = 1;

      float ux = x2 - x1, uy = y2 - y1;
      ux /= d, uy /= d;

      auto& linked_notes = ws.links[note.id];
      auto adjecent_note_iter =
          std::find(linked_notes.begin(), linked_notes.end(), other_note.id);
      if (adjecent_note_iter == linked_notes.end()) {
        fx += (c_rep / (d * d)) * ux;
        fy += (c_rep / (d * d)) * uy;
      } else {
        fx += log10f(d / ideal_length) * c_spring * ux;
        fy += log10f(d / ideal_length) * c_spring * uy;
      }
    }

    forces[note.id].first = fx;
    forces[note.id].second = fy;
  }

  for (const auto& note : ws.notes) {
    coordinates[note.id].first += forces[note.id].first * damping;
    coordinates[note.id].second += forces[note.id].second * damping;
  }
}
