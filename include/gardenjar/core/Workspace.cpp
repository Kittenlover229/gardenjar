#include "Workspace.hpp"

#include <fstream>
#include <iostream>

using namespace gardenjar::core;

Workspace::Workspace(std::filesystem::path path) : root(path), id_counter(1) {
  refresh();
}

void Workspace::refresh() {
  std::map<NoteID, std::vector<std::u8string>> links_to;

  for (auto entry : std::filesystem::recursive_directory_iterator(root)) {
    if (entry.is_regular_file()) {
      std::ifstream file;
      file.open(entry.path());
      std::string s = {std::istreambuf_iterator<char>(file),
                       std::istreambuf_iterator<char>()};

      std::cmatch match_out;
      std::u8string note_title =
          std::regex_search(s.c_str(), match_out, get_regex_matching_header())
              ? std::u8string((const char8_t*)match_out[1].str().c_str())
              : std::filesystem::path(entry.path())
                    .replace_extension("")
                    .filename()
                    .u8string();

      auto note_id = get_spare_id();

      auto workspace_path =
          std::filesystem::relative(entry.path(), root).u8string();
      std::u8string::const_iterator sep_iter;
      while (
          (sep_iter = std::find(workspace_path.cbegin(), workspace_path.cend(),
                                std::filesystem::path::preferred_separator)) !=
          workspace_path.cend()) {
        workspace_path.replace(sep_iter, sep_iter + 1, u8"/");
      }

      name_mappings[workspace_path] =
          name_mappings[entry.path().filename().u8string()] =
              name_mappings[note_title] = note_id;

      Note note = Note(note_id, note_title);
      for (match_out;
           std::regex_search(s.c_str(), match_out, get_regex_matching_link());
           s = match_out.suffix().str())
        links_to[note.id].push_back(
            std::u8string((const char8_t*)match_out[1].str().c_str()));

      notes.push_back(std::move(note));
    }
  }

  for (auto& note : notes) {
    for (const auto& other_name : links_to[note.id]) {
      auto other_id = name_mappings[other_name];
      note_links.push_back(std::make_pair(note.id, other_id));
    }
  }
}

NoteID Workspace::get_spare_id() { return id_counter++; }

std::regex& Workspace::get_regex_matching_link() const {
  static std::regex regex =
      std::regex("\\[\\[->(.+?)\\]\\]", std::regex_constants::ECMAScript);
  return regex;
}

std::regex& Workspace::get_regex_matching_header() const {
  static std::regex regex =
      std::regex("^#\\s+(.+)", std::regex_constants::ECMAScript);
  return regex;
}
