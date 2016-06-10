#ifndef TELEGRAM_TYPES_PHOTOSIZE_H_INCLUDED
#define TELEGRAM_TYPES_PHOTOSIZE_H_INCLUDED

namespace telegram {
namespace types {

struct photosize {
  /// See https://core.telegram.org/bots/api#photosize
  std::string file_id;                                                          // Unique identifier for this file
  int_fast32_t width     = 0;                                                   // Photo width
  int_fast32_t height    = 0;                                                   // Photo height
  int_fast32_t file_size = 0;                                                   // Optional. File size

  static photosize const from_ptree(boost::property_tree::ptree const &tree);
  static photosize const from_ptree(boost::property_tree::ptree const &tree, std::string const &path);
};

photosize const photosize::from_ptree(boost::property_tree::ptree const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws boost::property_tree::ptree_bad_path
  return photosize{tree.get("file_id", ""),
                   tree.get("width", 0),
                   tree.get("heght", 0),
                   tree.get("file_size", 0)};
}
photosize const photosize::from_ptree(boost::property_tree::ptree const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws boost::property_tree::ptree_bad_path
  return from_ptree(tree.get_child(path));
}

}
}

#endif // TELEGRAM_TYPES_PHOTOSIZE_H_INCLUDED
