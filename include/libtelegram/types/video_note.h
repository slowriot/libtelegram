#ifndef TELEGRAM_TYPES_VIDEO_NOTE_H_INCLUDED
#define TELEGRAM_TYPES_VIDEO_NOTE_H_INCLUDED

#include "libtelegram/config.h"

#include "helpers/optional.h"

namespace telegram::types {

struct video_note {
  /// See https://core.telegram.org/bots/api#videonote
  std::string file_id;                                                          // Unique identifier for this file
  int_fast32_t length   = 0;                                                    // Video width and height as defined by sender
  int_fast32_t duration = 0;                                                    // Optional. Duration of the video_note in seconds as defined by sender
  std::optional<photosize> thumb;                                               // Optional. Video thumbnail
  std::optional<int_fast32_t> file_size;                                        // Optional. File size (in bytes)

  static video_note const from_json(nlohmann::json const &tree);
  static video_note const from_json(nlohmann::json const &tree, std::string const &path);
};

inline video_note const video_note::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return video_note{tree.at("file_id"),
                    tree.at("length"),
                    tree.at("duration"),
                    helpers::make_optional_from_json<photosize>(tree, "thumb"),
                    helpers::make_optional_from_json<int32_t>(tree, "file_size")};
}
inline video_note const video_note::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_VIDEO_NOTE_H_INCLUDED
