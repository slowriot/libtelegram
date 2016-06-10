#ifndef TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
#define TELEGRAM_TYPES_OPTIONAL_H_INCLUDED

#include <experimental/optional>

template<typename T>
std::experimental::optional<T> make_optional(boost::property_tree::ptree const &tree, std::string const &path);

template<typename T>
std::experimental::optional<T> make_optional(boost::property_tree::ptree const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  auto const child_optional(tree.get_child_optional(path));                     // first try to get the path
  if(child_optional) {
    return T::from_ptree(*child_optional);                                      // we succeed, so create the target type from it and return it
  } else {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}

template<typename T>
std::experimental::optional<std::vector<T>> make_optional_vector(boost::property_tree::ptree const &tree, std::string const &path);

template<typename T>
std::experimental::optional<std::vector<T>> make_optional_vector(boost::property_tree::ptree const &tree, std::string const &path) {
  /// Attempt to read an array of objects of the specified type from the tree at the given path, and return them as a vector
  auto const child_optional(tree.get_child_optional(path));                     // first try to get the path
  std::cerr << "LibTelegram: optional_vector: trying to get a vector of type " << typeid(T).name() << std::endl;
  if(child_optional) {
    // iterate through the child nodes
    auto const &entries(*child_optional);
    if(entries.empty()) {
      std::cerr << "LibTelegram: optional_vector: DEBUG: entries at path " << path << " are empty" << std::endl;
      return std::experimental::nullopt;                                        // an empty array (?) so we fail
    }
    std::cerr << "LibTelegram: optional_vector: DEBUG: entries at path " << path << " have " << entries.size() << " entries " << std::endl;
    std::vector<T> out;
    out.reserve(entries.size());
    //auto const &end = entries.end();
    //for(auto it = entries.begin(); it != end; ++it) {
    for(auto const &it : entries) {
      out.emplace_back(T::from_ptree(it.second));
      std::cerr << "LibTelegram: optional_vector: DEBUG: emplaced " << out.size() << " items of " << typeid(T).name() << std::endl;
    }
    std::cerr << "LibTelegram: optional_vector: DEBUG: done" << std::endl;
    return out;
  } else {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}

#endif // TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
