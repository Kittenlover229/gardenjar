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
};

}  // namespace gardenjar::core

#endif
