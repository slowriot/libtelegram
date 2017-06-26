#ifndef TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
#define TELEGRAM_TYPES_OPTIONAL_H_INCLUDED

#include <experimental/optional>
#include <json.hpp>

namespace telegram::types {

template<typename T>
std::experimental::optional<T> make_optional(nlohmann::json const &tree, std::string const &path);
template<>
std::experimental::optional<std::string> make_optional(nlohmann::json const &tree, std::string const &path);
template<>
std::experimental::optional<bool> make_optional(nlohmann::json const &tree, std::string const &path);
template<>
std::experimental::optional<int32_t> make_optional(nlohmann::json const &tree, std::string const &path);
template<>
std::experimental::optional<int64_t> make_optional(nlohmann::json const &tree, std::string const &path);
template<typename T>
std::experimental::optional<std::vector<T>> make_optional_vector(nlohmann::json const &tree, std::string const &path);

template<typename T>
std::experimental::optional<T> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    auto const child_optional(tree.at(path));                                   // first try to get the path
    return T::from_json(child_optional);
  } catch(std::out_of_range &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<std::string> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<std::string>();                                    // try to get the path and the target as the right type
  } catch(std::out_of_range &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<int32_t> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<int32_t>();                                        // try to get the path and the target as the right type
  } catch(std::out_of_range &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<bool> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<bool>();                                           // try to get the path and the target as the right type
  } catch(std::out_of_range &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<int64_t> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<int64_t>();                                        // try to get the path and the target as the right type
  } catch(std::out_of_range &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}

template<typename T>
std::experimental::optional<std::vector<T>> make_optional_vector(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an array of objects of the specified type from the json at the given path, and return them as a vector
  try {
    auto const &entries(tree.at(path));                                         // try to get the path first
    if(entries.empty() || !entries.is_array()) {
      return std::experimental::nullopt;                                        // fail if it's an empty array, or not an array at all
    }
    std::vector<T> out;
    out.reserve(entries.size());
    for(auto const &it : entries) {                                             // iterate through the child nodes
      out.emplace_back(T::from_json(it));                                       // and populate the vector with the right constructed types
    }
    return out;
  } catch(std::out_of_range &e) {
    return std::experimental::nullopt;                                          // if we fail, return an empty optional
  }
}

}

#endif // TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
