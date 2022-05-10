#include "GraphViewWindow.hpp"

#include <imgui.h>
#include <math.h>

#include <iostream>

using namespace gardenjar::windowing;

GraphViewWindow::GraphViewWindow(core::Workspace& ws)
    : ws(ws),
      Window(u8"Graph View"),
      c_rep(2),
      ideal_length(100),
      c_spring(1),
      damping(1),
      node_fatness(1.5),
      show_controls(false),
      show_filters(false),
      show_grid(true),
      line_thickness(0.5),
      node_radius(2),
      dragged_note_id(0) {
  std::memset(search_query_buf, 0x00, sizeof(char8_t) * 64);
  this->refresh();
}

void GraphViewWindow::refresh() {
  ws.refresh();
  show_with_tag.clear();

  std::for_each(ws.tags.begin(), ws.tags.end(), [this](auto& tag_id_and_tag) {
    show_with_tag[tag_id_and_tag.first] = true;
  });

  coordinates.clear();
  x_offset = 0, y_offset = 0;
}

void GraphViewWindow::ImGuiFilterMenu() {
  if (show_filters) {
    if (ImGui::Begin("Filters", &show_filters)) {
      if (ImGui::TreeNode("Tag Filters")) {
        auto userdata = std::make_pair(&search_query_len, search_query_buf);
        ImGui::InputText(
            "Search Tags", (char*)search_query_buf, 63,
            ImGuiInputTextFlags_AutoSelectAll |
                ImGuiInputTextFlags_CallbackEdit,
            [](ImGuiInputTextCallbackData* data) {
              auto len_buff = (decltype(userdata)*)(data->UserData);
              auto& [len, buf] = *len_buff;
              *len = data->BufTextLen;
              return 0;
            },
            &userdata);

        if (ImGui::Button("Disable All")) {
          for (auto& tag : this->show_with_tag) {
            tag.second = false;
          }
        }
        ImGui::SameLine();
        if (ImGui::Button("Enable All")) {
          for (auto& tag : this->show_with_tag) {
            tag.second = true;
          }
        }

        ImGui::Separator();

        if (search_query_len == 0) {
          for (const auto& [id, tag] : ws.tags) {
            ImGui::Checkbox((const char*)tag.str.c_str(), &show_with_tag[id]);
          }
        } else {
          for (const auto& [id, tag] : ws.tags)
            if (tag.str.find(std::u8string(search_query_buf)) !=
                std::u8string::npos)
              ImGui::Checkbox((const char*)tag.str.c_str(), &show_with_tag[id]);
        }

        ImGui::TreePop();
      }
    }
    ImGui::End();
  }
}

void GraphViewWindow::ImGuiControlMenu() {
  if (show_controls) {
    ImGui::Begin("Controls", &show_controls);
    if (ImGui::Button("Reset")) {
      srand(time(NULL));
      coordinates.clear();
    }
    ImGui::SameLine();
    ImGui::Checkbox("Show Grid", &show_grid);
    ImGui::Separator();
    ImGui::SliderFloat("Ideal Length", &ideal_length, 30, 600);
    ImGui::SliderFloat("Damping", &damping, 0, 1);
    ImGui::SliderFloat("Repel", &c_rep, 1, 9);
    ImGui::SliderFloat("Spring", &c_spring, 1, 9);
    ImGui::Separator();
    ImGui::SliderFloat("Minimal Node Radius", &node_radius, 1, 2);
    ImGui::SliderFloat("Node Fatness", &node_fatness, 1, 1.5);
    ImGui::SliderFloat("Line Thickness", &line_thickness, 0.5, 10);
    ImGui::End();
  }
}

void GraphViewWindow::ImGuiOptionsMenu() {
  if (ImGui::BeginMenu("Options")) {
    ImGui::MenuItem("Show Filters", nullptr, &show_filters);
    ImGui::MenuItem("Show Controls", nullptr, &show_controls);
    if (ImGui::MenuItem("Refresh")) refresh();
    ImGui::EndMenu();
  }
}

void GraphViewWindow::ImGuiNotePopup(const core::Note& note) {
  ImGui::BeginTooltip();
  ImGui::Text((const char*)note.title.c_str());

  auto tag_count = note.tag_ids.size();
  if (tag_count > 0) {
    const char* fmt_str = tag_count == 1 ? "1 tag:" : "%d tags:";
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), fmt_str, tag_count);
    for (int i = 0; i < note.tag_ids.size(); i++) {
      auto tag_id = note.tag_ids[i];
      ImGui::TextColored(ImVec4(0.6, 0.6, 0.6, 1), "#%s",
                         ws.tags[tag_id].str.c_str());
    }
  }

  ImGui::EndTooltip();
}

bool GraphViewWindow::is_note_visible(core::NoteID id) {
  auto show_by_tags =
      std::any_of(ws.get_note_by_id(id)->tag_ids.cbegin(),
                  ws.get_note_by_id(id)->tag_ids.cend(),
                  [this](auto id) { return show_with_tag[id]; });

  return show_by_tags;
}

void GraphViewWindow::draw_impl(WindowManager& wm) {
  ImGuiOptionsMenu();
  ImGuiControlMenu();
  ImGuiFilterMenu();

  shape_graph();

  auto draw_list = ImGui::GetWindowDrawList();
  auto window_pos = ImGui::GetWindowPos();
  draw_list->ChannelsSplit(3);

  // Draw grid
  if (show_grid) {
    ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
    float GRID_SZ = 64.0f;
    ImVec2 win_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetWindowSize();
    for (float x = fmodf(x_offset, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
      draw_list->AddLine(ImVec2(x + win_pos.x, win_pos.y),
                         ImVec2(x + win_pos.x, canvas_sz.y + win_pos.y),
                         GRID_COLOR);
    for (float y = fmodf(y_offset, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
      draw_list->AddLine(ImVec2(win_pos.x, y + win_pos.y),
                         ImVec2(canvas_sz.x + win_pos.x, y + win_pos.y),
                         GRID_COLOR);
  }

  // Draw links
  draw_list->ChannelsSetCurrent(1);

  for (const auto& [note_id, other_note_ids] : ws.links) {
    if (!is_note_visible(note_id)) continue;

    auto [x1, y1] = coordinates[note_id];
    for (const auto& other_note_id : other_note_ids) {
      if (!is_note_visible(other_note_id)) continue;

      auto [x2, y2] = coordinates[other_note_id];
      draw_list->AddLine(
          ImVec2(x_offset + window_pos.x + x1, y_offset + window_pos.y + y1),
          ImVec2(x_offset + window_pos.x + x2, y_offset + window_pos.y + y2),
          IM_COL32(75, 75, 75, 255), line_thickness);
    }
  }

  // Draw notes
  draw_list->ChannelsSetCurrent(2);
  for (const auto& note : ws.notes) {
    if (!is_note_visible(note.id)) continue;

    auto neighbours_count = ws.links[note.id].size();

    auto [x, y] = coordinates[note.id];
    auto [mx, my] = ImGui::GetMousePos();

    auto d = std::sqrtf((mx - (x_offset + window_pos.x + x)) *
                            (mx - (x_offset + window_pos.x + x)) +
                        (my - (y_offset + window_pos.y + y)) *
                            (my - (y_offset + window_pos.y + y)));

    auto display_node_radius =
        node_radius * (1 + powf(node_fatness, neighbours_count));

    if (d <= display_node_radius * node_interaction_radius &&
        ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive()) {
      // Node is hovered
      draw_list->AddCircleFilled(
          ImVec2(x_offset + window_pos.x + x, y_offset + window_pos.y + y),
          display_node_radius, IM_COL32(200, 200, 200, 255));

      if (dragged_note_id == 0 && ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        dragged_note_id = note.id;
      } else {
        ImGuiNotePopup(note);
      }
    } else {
      // Node is not hovered
      draw_list->AddCircleFilled(
          ImVec2(x_offset + window_pos.x + x, y_offset + window_pos.y + y),
          display_node_radius, IM_COL32(100, 100, 100, 255));
    }
  }

  draw_list->ChannelsMerge();

  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
      ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
    if (dragged_note_id) {
      // You are dragging a node
      coordinates[dragged_note_id].first =
          ImGui::GetMousePos().x - x_offset - window_pos.x;
      coordinates[dragged_note_id].second =
          ImGui::GetMousePos().y - y_offset - window_pos.y;
    } else {
      // You are scrolling
      x_offset += ImGui::GetIO().MouseDelta.x;
      y_offset += ImGui::GetIO().MouseDelta.y;
    }
  }

  if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle)) {
    dragged_note_id = 0;
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
    if (!is_note_visible(note.id)) continue;

    float fx = 0, fy = 0;
    auto [x1, y1] = coordinates[note.id];

    for (const auto& other_note : ws.notes) {
      if (other_note.id == note.id) continue;
      if (!is_note_visible(other_note.id)) continue;

      auto [x2, y2] = coordinates[other_note.id];
      auto d = std::sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
      
      float ux = x2 - x1, uy = y2 - y1;
      ux /= d, uy /= d;

      auto& linked_notes = ws.links[note.id];
      auto adjecent_note_iter =
          std::find(linked_notes.begin(), linked_notes.end(), other_note.id);
      if (adjecent_note_iter == linked_notes.end()) {
        fx += (c_rep / (d * d)) * ux;
        fy += (c_rep / (d * d)) * uy;
      }
      fx += log10f(d / ideal_length) * c_spring * ux;
      fy += log10f(d / ideal_length) * c_spring * uy;
    }

    forces[note.id].first = fx;
    forces[note.id].second = fy;
  }

  for (const auto& note : ws.notes) {
    if (!is_note_visible(note.id)) continue;
    coordinates[note.id].first += forces[note.id].first * damping;
    coordinates[note.id].second += forces[note.id].second * damping;
  }
}
