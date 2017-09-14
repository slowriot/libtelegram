#ifndef TELEGRAM_TYPES_PHOTOSIZE_H_INCLUDED
#define TELEGRAM_TYPES_PHOTOSIZE_H_INCLUDED

#include "helpers/optional.h"

namespace telegram::types {

struct photosize {
  /// See https://core.telegram.org/bots/api#photosize
  std::string file_id;                                                          // Unique identifier for this file
  int_fast32_t width  = 0;                                                      // Photo width
  int_fast32_t height = 0;                                                      // Photo height
  std::optional<int_fast32_t> file_size;                                        // Optional. File size (in bytes)

  static photosize const from_json(nlohmann::json const &tree);
  static photosize const from_json(nlohmann::json const &tree, std::string const &path);
};

inline photosize const photosize::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return photosize{tree.at("file_id"),
                   tree.at("width"),
                   tree.at("height"),
                   helpers::make_optional_from_json<int32_t>(tree, "file_size")};
}
inline photosize const photosize::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_PHOTOSIZE_H_INCLUDED
