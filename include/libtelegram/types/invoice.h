#ifndef TELEGRAM_TYPES_INVOICE_H_INCLUDED
#define TELEGRAM_TYPES_INVOICE_H_INCLUDED

namespace telegram::types {

struct invoice {
  /// See https://core.telegram.org/bots/api#invoice
  std::string title;                                                            // Product name
  std::string description;                                                      // Product description
  std::string start_parameter;                                                  // Unique bot deep-linking parameter that can be used to generate this invoice
  std::string currency;                                                         // Three-letter ISO 4217 currency code - see https://core.telegram.org/bots/payments#supported-currencies
  int_fast32_t total_amount = 0;                                                // Total price in the smallest units of the currency (integer, not float/double). For example, for a price of US$ 1.45 pass amount = 145. See the exp parameter in https://core.telegram.org/bots/payments/currencies.json, it shows the number of digits past the decimal point for each currency (2 for the majority of currencies).

  static invoice const from_json(nlohmann::json const &tree);
  static invoice const from_json(nlohmann::json const &tree, std::string const &path);
};

inline invoice const invoice::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return invoice{tree.at("title"),
                 tree.at("description"),
                 tree.at("start_parameter"),
                 tree.at("currency"),
                 tree.at("total_amount")};
}
inline invoice const invoice::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_INVOICE_H_INCLUDED
