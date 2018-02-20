#ifndef TELEGRAM_TYPES_SHIPPING_ADDRESS_H_INCLUDED
#define TELEGRAM_TYPES_SHIPPING_ADDRESS_H_INCLUDED

#include "libtelegram/config.h"

namespace telegram::types {

struct shipping_address {
  /// See https://core.telegram.org/bots/api#shippingaddress
  std::string country_code;                                                     // ISO 3166-1 alpha-2 country code
  std::string state;                                                            // State, if applicable
  std::string city;                                                             // City
  std::string street_line1;                                                     // First line for the address
  std::string street_line2;                                                     // Second line for the address
  std::string post_code;                                                        // Address post code

  static shipping_address const from_json(nlohmann::json const &tree);
  static shipping_address const from_json(nlohmann::json const &tree, std::string const &path);
};

inline shipping_address const shipping_address::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return shipping_address{tree.at("country_code"),
                          tree.at("state"),
                          tree.at("city"),
                          tree.at("street_line1"),
                          tree.at("street_line2"),
                          tree.at("post_code")};
}
inline shipping_address const shipping_address::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_SHIPPING_ADDRESS_H_INCLUDED
