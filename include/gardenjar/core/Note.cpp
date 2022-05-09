#include "Note.hpp"

using namespace gardenjar::core;

Note::Note(NoteID id, std::u8string title, std::vector<TagID>&& tags)
    : id(id), title(title), tag_ids(std::move(tags)) {}
