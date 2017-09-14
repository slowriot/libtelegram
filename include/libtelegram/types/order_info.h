#ifndef TELEGRAM_TYPES_ORDER_INFO_H_INCLUDED
#define TELEGRAM_TYPES_ORDER_INFO_H_INCLUDED

#include "shipping_address.h"

namespace telegram::types {

struct order_info {
  /// See https://core.telegram.org/bots/api#orderinfo
  std::optional<std::string> name;                                              // Optional. User name
  std::optional<std::string> phone_number;                                      // Optional. User's phone number
  std::optional<std::string> email;                                             // Optional. User email
  std::optional<types::shipping_address> shipping_address;                      // Optional. User shipping address

  static order_info const from_json(nlohmann::json const &tree);
  static order_info const from_json(nlohmann::json const &tree, std::string const &path);
};

inline order_info const order_info::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return order_info{helpers::make_optional_from_json<std::string>(tree, "name"),
                    helpers::make_optional_from_json<std::string>(tree, "phone_number"),
                    helpers::make_optional_from_json<std::string>(tree, "email"),
                    helpers::make_optional_from_json<types::shipping_address>(tree, "shipping_address")};
}
inline order_info const order_info::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_ORDER_INFO_H_INCLUDED
