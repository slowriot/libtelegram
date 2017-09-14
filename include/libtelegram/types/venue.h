#ifndef TELEGRAM_TYPES_VENUE_H_INCLUDED
#define TELEGRAM_TYPES_VENUE_H_INCLUDED

#include "venue.h"

namespace telegram::types {

struct venue {
  /// See https://core.telegram.org/bots/api#venue
  types::location location;                                                     // Venue location
  std::string title;                                                            // Name of the venue
  std::string address;                                                          // Address of the venue
  std::optional<std::string> foursquare_id;                                     // Optional. Foursquare identifier of the venue

  static venue const from_json(nlohmann::json const &tree);
  static venue const from_json(nlohmann::json const &tree, std::string const &path);
};

inline venue const venue::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return venue{types::location::from_json(tree, "location"),
               tree.at("title"),
               tree.at("address"),
               helpers::make_optional_from_json<std::string>(tree, "foursquare_id")};
}
inline venue const venue::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_VENUE_H_INCLUDED
