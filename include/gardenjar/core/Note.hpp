#ifndef GARDENJAR_H__CORE__NOTE
#define GARDENJAR_H__CORE__NOTE

#include <vector>
#include <string>

namespace gardenjar::core {

using NoteID = unsigned long;

struct Note {
  explicit Note(NoteID id, std::u8string title);

  NoteID id;
  std::u8string title;
  std::vector<std::u8string> links_to;
};

}  // namespace gardenjar::core

#endif
