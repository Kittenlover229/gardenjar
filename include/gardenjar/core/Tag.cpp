#include "Tag.hpp"

using namespace gardenjar::core;

Tag::Tag() : str(), id() {}

Tag::Tag(std::u8string str, TagID id) : str(str), id(id) {}
