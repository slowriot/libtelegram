#ifndef TELEGRAM_TYPES_SUCCESSFUL_PAYMENT_H_INCLUDED
#define TELEGRAM_TYPES_SUCCESSFUL_PAYMENT_H_INCLUDED

#include "helpers/optional.h"
#include "order_info.h"

namespace telegram::types {

struct successful_payment {
  /// See https://core.telegram.org/bots/api#successfulpayment
  std::string currency;                                                         // Three-letter ISO 4217 currency code - see https://core.telegram.org/bots/payments#supported-currencies
  int_fast32_t total_amount = 0;                                                // Total price in the smallest units of the currency (integer, not float/double). For example, for a price of US$ 1.45 pass amount = 145. See the exp parameter in https://core.telegram.org/bots/payments/currencies.json, it shows the number of digits past the decimal point for each currency (2 for the majority of currencies).
  std::string invoice_payload;                                                  // Bot specified invoice payload
  std::optional<std::string> shipping_option_id;                                // Optional. Identifier of the shipping option chosen by the user
  std::optional<types::order_info> order_info;                                  // Optional. Order info provided by the user
  std::string telegram_payment_charge_id;                                       // Telegram payment identifier
  std::string provider_payment_charge_id;                                       // Provider payment identifier

  static successful_payment const from_json(nlohmann::json const &tree);
  static successful_payment const from_json(nlohmann::json const &tree, std::string const &path);
};

inline successful_payment const successful_payment::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return successful_payment{tree.at("currency"),
                            tree.at("total_amount"),
                            tree.at("invoice_payload"),
                            helpers::make_optional_from_json<std::string>(tree, "shipping_option_id"),
                            helpers::make_optional_from_json<types::order_info>(tree, "order_info"),
                            tree.at("telegram_payment_charge_id"),
                            tree.at("provider_payment_charge_id")};
}
inline successful_payment const successful_payment::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_SUCCESSFUL_PAYMENT_H_INCLUDED
