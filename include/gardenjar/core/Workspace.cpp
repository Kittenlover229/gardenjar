#include "Workspace.hpp"

#include <fstream>
#include <functional>
#include <iostream>

using namespace gardenjar::core;

Workspace::Workspace(std::filesystem::path path)
    : root(path), note_id_counter(1), tag_id_counter(1) {
  refresh();
}

void forall_regex_matches_in_str(
    std::string_view str, const std::regex& regex,
    std::function<void(std::u8string&&)> callback) {
  std::cmatch match_out;
  std::string file_contents_copy(str);
  for (match_out;
       std::regex_search(file_contents_copy.c_str(), match_out, regex);
       file_contents_copy = match_out.suffix().str())
    callback(std::u8string((const char8_t*)match_out[1].str().c_str()));
}

void Workspace::refresh() {
  links.clear();
  note_id_counter = 1;
  tag_id_counter = 1;
  name_mappings.clear();
  notes.clear();

  std::map<NoteID, std::vector<std::u8string>> links_to;

  for (auto entry : std::filesystem::recursive_directory_iterator(root)) {
    if (entry.is_regular_file()) {
      std::ifstream file;
      file.open(entry.path());
      std::string file_contents = {std::istreambuf_iterator<char>(file),
                                   std::istreambuf_iterator<char>()};

      auto note_path = entry.path();
      std::cmatch match_out;
      std::u8string note_title =
          std::regex_search(file_contents.c_str(), match_out,
                            get_regex_matching_header())
              ? std::u8string((const char8_t*)match_out[1].str().c_str())
              : std::filesystem::path(note_path)
                    .replace_extension("")
                    .filename()
                    .u8string();

      auto note_id = get_spare_note_id();

      name_mappings[system_path_to_workspace_path(note_path)] =
          name_mappings[note_path.filename().u8string()] =
              name_mappings[note_title] = note_id;

      forall_regex_matches_in_str(
          file_contents, get_regex_matching_link(),
          [&](auto str) { links_to[note_id].push_back(str); });

      std::vector<TagID> note_tags;
      forall_regex_matches_in_str(
          file_contents, get_regex_matching_tag(),
          [&](auto str) { note_tags.push_back(get_tag_id_by_name(str)); });

      notes.push_back(Note(note_id, note_title, std::move(note_tags)));
    }
  }

  for (auto& note : notes)
    for (const auto& other_name : links_to[note.id])
      links[note.id].push_back(get_note_id_by_name(other_name));
}

NoteID Workspace::get_spare_note_id() { return note_id_counter++; }
TagID Workspace::get_spare_tag_id() { return tag_id_counter++; }

const std::regex& Workspace::get_regex_matching_link() const {
  static std::regex regex =
      std::regex("\\[\\[->(.+?)\\]\\]", std::regex_constants::ECMAScript);
  return regex;
}

const std::regex& Workspace::get_regex_matching_header() const {
  static std::regex regex =
      std::regex("^#\\s+(.+)", std::regex_constants::ECMAScript);
  return regex;
}

const std::regex& Workspace::get_regex_matching_tag() const {
  static std::regex regex = std::regex("#([^\\s!@#$%^&*(),.?\":{}|<>]+)");
  return regex;
}

NoteID Workspace::get_note_id_by_name(std::u8string name) {
  return name_mappings[name];
}

TagID Workspace::get_tag_id_by_name(std::u8string name) {
  // Find the tag if it exists
  auto tag_iter = std::find_if(tags.cbegin(), tags.cend(), [&](const auto& it) {
    return it.second.str == name;
  });

  if (tag_iter != tags.cend()) {
    // Return existent tag
    return tag_iter->first;
  } else {
    // Insert new tag and return it's id
    auto tag = Tag(name, get_spare_tag_id());
    auto id = tag.id;
    tags[id] = tag;
    return id;
  }
}

std::u8string Workspace::system_path_to_workspace_path(
    std::filesystem::path path) const {
  auto workspace_path = std::filesystem::relative(path, root).u8string();
  std::u8string::const_iterator sep_iter;
  while ((sep_iter = std::find(workspace_path.cbegin(), workspace_path.cend(),
                               std::filesystem::path::preferred_separator)) !=
         workspace_path.cend()) {
    workspace_path.replace(sep_iter, sep_iter + 1, u8"/");
  }

  return workspace_path;
}

Note* Workspace::get_note_by_id(NoteID id) {
  for (auto& note : notes)
    if (note.id == id) return &note;
  return nullptr;
}
