#ifndef TELEGRAM_TYPES_CONTACT_H_INCLUDED
#define TELEGRAM_TYPES_CONTACT_H_INCLUDED

#include "libtelegram/config.h"

namespace telegram::types {

struct contact {
  /// See https://core.telegram.org/bots/api#contact
  std::string phone_number;                                                     // Contact's phone number
  std::string first_name;                                                       // Contact's first name
  std::optional<std::string> last_name;                                         // Optional. Contact's last name
  std::optional<int_fast32_t> user_id;                                          // Optional. Contact's user identifier in Telegram


  static contact const from_json(nlohmann::json const &tree);
  static contact const from_json(nlohmann::json const &tree, std::string const &path);
};

inline contact const contact::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return contact{tree.at("phone_number"),
                 tree.at("first_name"),
                 helpers::make_optional_from_json<std::string>(tree, "last_name"),
                 helpers::make_optional_from_json<int32_t>(tree, "user_id")};
}
inline contact const contact::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_CONTACT_H_INCLUDED
