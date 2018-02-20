#ifndef TELEGRAM_TYPES_HELPERS_SHARED_H_INCLUDED
#define TELEGRAM_TYPES_HELPERS_SHARED_H_INCLUDED

#include "libtelegram/config.h"

#include <memory>
#include <json.hpp>

namespace telegram::types::helpers {

template<typename T>
inline std::shared_ptr<T> make_shared_from_json(nlohmann::json const &tree, std::string const &path);

template<typename T>
inline std::shared_ptr<T> make_shared_from_json(nlohmann::json const &tree, std::string const &path) {
  /// Attempt to read an object of the specified type from the tree at the given path, and return it - useful for circular dependencies, such as messages containing other messages
  try {
    auto const child_shared_ptr(tree.at(path));                                 // first try to get the path
    return std::make_shared<T>(T::from_json(child_shared_ptr));
  } catch(std::out_of_range &e) {
    return nullptr;                                                             // we fail, so return an empty shared_ptr
  }
}

}

#endif // TELEGRAM_TYPES_HELPERS_SHARED_H_INCLUDED
