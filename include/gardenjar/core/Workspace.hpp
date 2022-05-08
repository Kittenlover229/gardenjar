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

  NoteID get_spare_note_id();
  TagID get_spare_tag_id();
  std::regex& get_regex_matching_link() const;
  std::regex& get_regex_matching_header() const;
  std::regex& get_regex_matching_tag() const;

  std::map<std::u8string, NoteID> name_mappings;
  std::map<NoteID, std::vector<NoteID>> links;
  std::vector<Note> notes;
  std::map<TagID, Tag> tags;

 protected:
  std::filesystem::path root;

  std::atomic<NoteID> note_id_counter;
  std::atomic<TagID> tag_id_counter;
};

}  // namespace gardenjar::core

#endif
