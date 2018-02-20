#ifndef TELEGRAM_TYPES_USER_PROFILE_PHOTOS_H_INCLUDED
#define TELEGRAM_TYPES_USER_PROFILE_PHOTOS_H_INCLUDED

#include "libtelegram/config.h"

#include "photosize.h"

namespace telegram::types {

struct user_profile_photos {
  /// See https://core.telegram.org/bots/api#userprofilephotos
  int_fast32_t total_count = 0;                                                 // Total number of profile pictures the target user has
  std::vector<std::vector<photosize>> photos;                                   // Requested profile pictures (in up to 4 sizes each)

  static user_profile_photos const from_json(nlohmann::json const &tree);
  static user_profile_photos const from_json(nlohmann::json const &tree, std::string const &path);
};

inline user_profile_photos const user_profile_photos::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  // decode a two-dimensional array of images
  auto const &photo_entries1(tree.at("photos"));                                // try to get the path first
  std::vector<std::vector<photosize>> these_photos;
  these_photos.reserve(photo_entries1.size());
  for(auto const &photo_entries2 : photo_entries1) {                            // iterate through the first level child nodes
    these_photos.emplace_back();
    these_photos.back().reserve(photo_entries2.size());
    for(auto const &it : photo_entries2) {                                      // iterate through the second level child nodes
      these_photos.back().emplace_back(photosize::from_json(it));               // and populate the vector with the right constructed types
    }
  }
  return user_profile_photos{tree.at("total_count"),
                             these_photos};
}
inline user_profile_photos const user_profile_photos::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_USER_PROFILE_PHOTOS_H_INCLUDED
