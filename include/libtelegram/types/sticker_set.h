#ifndef TELEGRAM_TYPES_STICKER_SET_H_INCLUDED
#define TELEGRAM_TYPES_STICKER_SET_H_INCLUDED

#include "libtelegram/config.h"

#include "sticker.h"

namespace telegram::types {

struct sticker_set {
  /// See https://core.telegram.org/bots/api#sticker_set
  std::string name;                                                             // Sticker set name
  std::string title;                                                            // Sticker set title
  bool contains_masks = false;                                                  // True, if the sticker set contains masks
  std::vector<sticker> stickers;

  static sticker_set const from_json(nlohmann::json const &tree);
  static sticker_set const from_json(nlohmann::json const &tree, std::string const &path);
};

inline sticker_set const sticker_set::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  auto const &entries(tree.at("stickers"));                                     // try to get the path first
  std::vector<sticker> these_stickers;
  these_stickers.reserve(entries.size());
  for(auto const &it : entries) {                                               // iterate through the child nodes
    these_stickers.emplace_back(sticker::from_json(it));                        // and populate the vector with the right constructed types
  }

  return sticker_set{tree.at("name"),
                     tree.at("title"),
                     tree.at("contains_masks"),
                     these_stickers};
}
inline sticker_set const sticker_set::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_STICKER_SET_H_INCLUDED
