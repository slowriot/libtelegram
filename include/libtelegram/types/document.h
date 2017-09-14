#ifndef TELEGRAM_TYPES_DOCUMENT_H_INCLUDED
#define TELEGRAM_TYPES_DOCUMENT_H_INCLUDED

#include "photosize.h"

namespace telegram::types {

struct document {
  /// See https://core.telegram.org/bots/api#document
  std::string file_id;                                                          // Unique file identifier
  std::optional<photosize> thumb;                                               // Optional. Document thumbnail as defined by sender
  std::optional<std::string> file_name;                                         // Optional. Original filename as defined by sender
  std::optional<std::string> mime_type;                                         // Optional. MIME type of the file as defined by sender
  std::optional<int_fast32_t> file_size;                                        // Optional. File size (in bytes)

  static document const from_json(nlohmann::json const &tree);
  static document const from_json(nlohmann::json const &tree, std::string const &path);
};

inline document const document::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return document{tree.at("file_id"),
                  helpers::make_optional_from_json<photosize>(tree, "thumb"),
                  helpers::make_optional_from_json<std::string>(tree, "file_name"),
                  helpers::make_optional_from_json<std::string>(tree, "mime_type"),
                  helpers::make_optional_from_json<int32_t>(tree, "file_size")};
}
inline document const document::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_DOCUMENT_H_INCLUDED
