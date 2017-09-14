#ifndef TELEGRAM_TYPES_STICKER_H_INCLUDED
#define TELEGRAM_TYPES_STICKER_H_INCLUDED

#include "mask_position.h"

namespace telegram::types {

struct sticker {
  /// See https://core.telegram.org/bots/api#sticker
  std::string file_id;                                                          // Unique identifier for this file
  int_fast32_t width  = 0;                                                      // Sticker width
  int_fast32_t height = 0;                                                      // Sticker height
  std::optional<photosize> thumb;                                               // Optional. Sticker thumbnail in the .webp or .jpg format
  std::optional<std::string> emoji;                                             // Optional. Emoji associated with the sticker
  std::optional<std::string> set_name;                                          // Optional. Name of the sticker set to which the sticker belongs
  std::optional<types::mask_position> mask_position;                            // Optional. For mask stickers, the position where the mask should be placed
  std::optional<int_fast32_t> file_size;                                        // Optional. File size (in bytes)

  static sticker const from_json(nlohmann::json const &tree);
  static sticker const from_json(nlohmann::json const &tree, std::string const &path);
};

inline sticker const sticker::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return sticker{tree.at("file_id"),
                 tree.at("width"),
                 tree.at("height"),
                 helpers::make_optional_from_json<photosize>(tree, "thumb"),
                 helpers::make_optional_from_json<std::string>(tree, "emoji"),
                 helpers::make_optional_from_json<std::string>(tree, "set_name"),
                 helpers::make_optional_from_json<types::mask_position>(tree, "mask_position"),
                 helpers::make_optional_from_json<int32_t>(tree, "file_size")};
}
inline sticker const sticker::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_STICKER_H_INCLUDED
