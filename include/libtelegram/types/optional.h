#ifndef TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
#define TELEGRAM_TYPES_OPTIONAL_H_INCLUDED

#include <experimental/optional>
#include <json.hpp>

namespace telegram {
namespace types {

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
  } catch(std::exception &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<std::string> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<std::string>();                                    // try to get the path and the target as the right type
  } catch(std::exception &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<int32_t> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<int32_t>();                                       // try to get the path and the target as the right type
  } catch(std::exception &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<bool> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<bool>();                                           // try to get the path and the target as the right type
  } catch(std::exception &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}
template<>
std::experimental::optional<int64_t> make_optional(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it
  try {
    return tree.at(path).get<int64_t>();                                       // try to get the path and the target as the right type
  } catch(std::exception &e) {
    return std::experimental::nullopt;                                          // we fail, so return an empty optional
  }
}

template<typename T>
std::experimental::optional<std::vector<T>> make_optional_vector(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an array of objects of the specified type from the json at the given path, and return them as a vector
  // TODO: array version
  return std::experimental::nullopt;
}

}
}

#endif // TELEGRAM_TYPES_OPTIONAL_H_INCLUDED
