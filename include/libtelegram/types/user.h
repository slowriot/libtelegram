#ifndef TELEGRAM_TYPES_USER_H_INCLUDED
#define TELEGRAM_TYPES_USER_H_INCLUDED

namespace telegram {
namespace types {

struct user {
  int_fast32_t id = 0;                                                          // Unique identifier for this user or bot
  std::string first_name;                                                       // User‘s or bot’s first name
  std::string last_name;                                                        // User‘s or bot’s last name
  std::string username;                                                         // User‘s or bot’s username

  static user const from_ptree(boost::property_tree::ptree const &tree);
  static user const from_ptree(boost::property_tree::ptree const &tree, std::string const &path);
};

user const user::from_ptree(boost::property_tree::ptree const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws boost::property_tree::ptree_bad_path
  return user{tree.get("id", 0),
              tree.get("first_name", ""),
              tree.get("last_name", ""),
              tree.get("username", "")};
}
user const user::from_ptree(boost::property_tree::ptree const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws boost::property_tree::ptree_bad_path
  return from_ptree(tree.get_child(path));
}

}
}

#endif // TELEGRAM_TYPES_USER_H_INCLUDED
