#ifndef TELEGRAM_TYPES_AUDIO_H_INCLUDED
#define TELEGRAM_TYPES_AUDIO_H_INCLUDED

#include "helpers/optional.h"

namespace telegram::types {

struct audio {
  /// See https://core.telegram.org/bots/api#audio
  std::string file_id;                                                          // Unique identifier for this file
  int_fast32_t duration = 0;                                                    // Duration of the audio in seconds as defined by sender
  std::optional<std::string> performer;                                         // Optional. Performer of the audio as defined by sender or by audio tags
  std::optional<std::string> title;                                             // Optional. Title of the audio as defined by sender or by audio tags
  std::optional<std::string> mime_type;                                         // Optional. MIME type of the file as defined by sender
  std::optional<int_fast32_t> file_size;                                        // Optional. File size (in bytes)

  static audio const from_json(nlohmann::json const &tree);
  static audio const from_json(nlohmann::json const &tree, std::string const &path);
};

inline audio const audio::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return audio{tree.at("file_id"),
               tree.at("duration"),
               helpers::make_optional_from_json<std::string>(tree, "performer"),
               helpers::make_optional_from_json<std::string>(tree, "title"),
               helpers::make_optional_from_json<std::string>(tree, "mime_type"),
               helpers::make_optional_from_json<int32_t>(tree, "file_size")};
}
inline audio const audio::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_AUDIO_H_INCLUDED
