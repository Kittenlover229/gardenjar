#ifndef GARDENJAR_H__CORE__TAG
#define GARDENJAR_H__CORE__TAG

#include <string>

namespace gardenjar::core {

using TagID = unsigned int;

struct Tag {
  Tag();
  Tag(std::u8string, TagID);

  TagID id;
  std::u8string str;
};

}  // namespace gardenjar::core

#endif
