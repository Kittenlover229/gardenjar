#ifndef GARDENJAR_H__CORE__WORKSPACE
#define GARDENJAR_H__CORE__WORKSPACE

#include <filesystem>
#include <map>
#include <regex>
#include <tuple>
#include <vector>

#include "Note.hpp"

namespace gardenjar::core {

struct Workspace {
  explicit Workspace(std::filesystem::path path);

  void refresh();

  Note* get_note_by_id(NoteID);
  TagID get_tag_id_by_name(std::u8string);
  NoteID get_note_id_by_name(std::u8string);
  NoteID get_spare_note_id();
  TagID get_spare_tag_id();

  std::u8string system_path_to_workspace_path(std::filesystem::path) const;
  const std::regex& get_regex_matching_link() const;
  const std::regex& get_regex_matching_header() const;
  const std::regex& get_regex_matching_tag() const;

  std::map<std::u8string, NoteID> name_mappings;
  std::map<NoteID, std::vector<NoteID>> links;
  std::vector<Note> notes;
  std::map<TagID, Tag> tags;

  inline static const char8_t* supported_extensions[] = {u8".txt", u8".md"};

 protected:
  std::filesystem::path root;

  std::atomic<NoteID> note_id_counter;
  std::atomic<TagID> tag_id_counter;
};

}  // namespace gardenjar::core

#endif
