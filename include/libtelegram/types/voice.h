#ifndef TELEGRAM_TYPES_VOICE_H_INCLUDED
#define TELEGRAM_TYPES_VOICE_H_INCLUDED

#include "libtelegram/config.h"

#include "helpers/optional.h"

namespace telegram::types {

struct voice {
  /// See https://core.telegram.org/bots/api#voice
  std::string file_id;                                                          // Unique identifier for this file
  int_fast32_t duration = 0;                                                    // Optional. Duration of the audio in seconds as defined by sender
  std::optional<std::string> mime_type;                                         // Optional. MIME type of the file as defined by sender
  std::optional<int_fast32_t> file_size;                                        // Optional. File size (in bytes)

  static voice const from_json(nlohmann::json const &tree);
  static voice const from_json(nlohmann::json const &tree, std::string const &path);
};

inline voice const voice::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return voice{tree.at("file_id"),
               tree.at("duration"),
               helpers::make_optional_from_json<std::string>(tree, "mime_type"),
               helpers::make_optional_from_json<int32_t>(tree, "file_size")};
}
inline voice const voice::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_VOICE_H_INCLUDED
