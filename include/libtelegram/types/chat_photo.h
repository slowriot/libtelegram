#ifndef TELEGRAM_TYPES_CHAT_PHOTO_H_INCLUDED
#define TELEGRAM_TYPES_CHAT_PHOTO_H_INCLUDED

namespace telegram::types {

struct chat_photo {
  /// See https://core.telegram.org/bots/api#chat_photo
  std::string small_file_id;                                                    // Unique file identifier of small (160x160) chat photo. This file_id can be used only for photo download.
  std::string big_file_id;                                                      // Unique file identifier of big (640x640) chat photo. This file_id can be used only for photo download.

  static chat_photo const from_json(nlohmann::json const &tree);
  static chat_photo const from_json(nlohmann::json const &tree, std::string const &path);
};

inline chat_photo const chat_photo::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return chat_photo{tree.at("small_file_id"),
                    tree.at("big_file_id")};
}
inline chat_photo const chat_photo::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_CHAT_PHOTO_H_INCLUDED
