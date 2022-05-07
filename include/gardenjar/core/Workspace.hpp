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

  NoteID get_spare_id();
  std::regex& get_regex_matching_link() const;
  std::regex& get_regex_matching_header() const;

 protected:
  std::filesystem::path root;

  std::map<std::u8string, NoteID> name_mappings;
  std::vector<std::pair<NoteID, NoteID>> note_links;
  std::vector<Note> notes;

  std::atomic<NoteID> id_counter;
};

}  // namespace gardenjar::core

#endif
