#ifndef TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
#define TELEGRAM_TYPES_OPTIONAL_H_INCLUDED

#include <experimental/optional>

template<typename T>
std::experimental::optional<T> make_optional(boost::property_tree::ptree const &tree, std::string const &path);

template<typename T>
std::experimental::optional<T> make_optional(boost::property_tree::ptree const &tree, std::string const &path) {
  auto const child_optional(tree.get_child_optional(path));                     // first try to get the path
  if(child_optional) {
    return T::from_ptree(*child_optional);                                      // we succeed, so create the target type from it and return it
  } else {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}

#endif // TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
