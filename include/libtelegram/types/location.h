#ifndef TELEGRAM_TYPES_LOCATION_H_INCLUDED
#define TELEGRAM_TYPES_LOCATION_H_INCLUDED

#include "libtelegram/config.h"

namespace telegram::types {

struct location {
  /// See https://core.telegram.org/bots/api#location
  float latitude  = 0.0f;                                                       // Latitude as defined by sender
  float longitude = 0.0f;                                                       // Longitude as defined by sender

  static location const from_json(nlohmann::json const &tree);
  static location const from_json(nlohmann::json const &tree, std::string const &path);
};

inline location const location::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return location{tree.at("latitude"),
                  tree.at("longitude")};
}
inline location const location::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_LOCATION_H_INCLUDED
