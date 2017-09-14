#ifndef TELEGRAM_TYPES_USER_H_INCLUDED
#define TELEGRAM_TYPES_USER_H_INCLUDED

#include "helpers/optional.h"

namespace telegram::types {

struct user {
  int_fast32_t id = 0;                                                          // Unique identifier for this user or bot
  bool is_bot = false;                                                          // True, if this user is a bot
  std::string first_name;                                                       // User‘s or bot’s first name
  std::optional<std::string> last_name;                                         // Optional. User‘s or bot’s last name
  std::optional<std::string> username;                                          // Optional. User‘s or bot’s username
  std::optional<std::string> language_code;                                     // Optional. IETF language tag of the user's language.  See https://en.wikipedia.org/wiki/IETF_language_tag

  static user const from_json(nlohmann::json const &tree);
  static user const from_json(nlohmann::json const &tree, std::string const &path);
};

inline user const user::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return user{tree.at("id"),
              tree.at("is_bot"),
              tree.at("first_name"),
              helpers::make_optional_from_json<std::string>(tree, "last_name"),
              helpers::make_optional_from_json<std::string>(tree, "username"),
              helpers::make_optional_from_json<std::string>(tree, "language_code")};
}
inline user const user::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_USER_H_INCLUDED
