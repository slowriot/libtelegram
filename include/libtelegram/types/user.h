#ifndef TELEGRAM_TYPES_USER_H_INCLUDED
#define TELEGRAM_TYPES_USER_H_INCLUDED

namespace telegram::types {

struct user {
  int_fast32_t id = 0;                                                          // Unique identifier for this user or bot
  std::string first_name;                                                       // User‘s or bot’s first name
  std::experimental::optional<std::string> last_name;                           // User‘s or bot’s last name
  std::experimental::optional<std::string> username;                            // User‘s or bot’s username

  static user const from_json(nlohmann::json const &tree);
  static user const from_json(nlohmann::json const &tree, std::string const &path);
};

user const user::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return user{tree.at("id"),
              tree.at("first_name"),
              make_optional<std::string>(tree, "last_name"),
              make_optional<std::string>(tree, "username")};
}
user const user::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_USER_H_INCLUDED
