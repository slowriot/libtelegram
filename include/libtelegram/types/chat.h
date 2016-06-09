#ifndef TELEGRAM_TYPES_CHAT_H_INCLUDED
#define TELEGRAM_TYPES_CHAT_H_INCLUDED

namespace telegram {
namespace types {

struct chat {
  enum class types {
    PRIVATE,
    GROUP,
    SUPERGROUP,
    CHANNEL,
    UNKNOWN
  };

  int_fast64_t id = 0;
  types type = types::UNKNOWN;
  std::string title;
  std::string username;
  std::string first_name;
  std::string last_name;

  static chat const from_ptree(boost::property_tree::ptree const &tree);
  static chat const from_ptree(boost::property_tree::ptree const &tree, std::string const &path);
};

chat const chat::from_ptree(boost::property_tree::ptree const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws boost::property_tree::ptree_bad_path
  types this_type;
  std::string const type_string(tree.get("type", ""));
  if(       type_string == "private") {
    this_type = types::PRIVATE;
  } else if(type_string == "group") {
    this_type = types::GROUP;
  } else if(type_string == "supergroup") {
    this_type = types::SUPERGROUP;
  } else if(type_string == "channel") {
    this_type = types::CHANNEL;
  } else {
    this_type = types::UNKNOWN;
  }
  chat result{tree.get<int_fast64_t>("id", 0),
              this_type,
              tree.get("title", ""),
              tree.get("username", ""),
              tree.get("first_name", ""),
              tree.get("last_name", "")};
  return result;
}
chat const chat::from_ptree(boost::property_tree::ptree const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws boost::property_tree::ptree_bad_path
  return from_ptree(tree.get_child(path));
}

}
}

#endif // TELEGRAM_TYPES_CHAT_H_INCLUDED
