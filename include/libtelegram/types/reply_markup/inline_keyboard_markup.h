#ifndef TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_MARKUP_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_MARKUP_H_INCLUDED

#include "base.h"
#include "inline_keyboard_button.h"

namespace telegram::types::reply_markup {

struct inline_keyboard_markup final : base<inline_keyboard_markup> {
  /// See https://core.telegram.org/bots/api/#inlinekeyboardmarkup
  std::vector<std::vector<inline_keyboard_button>> keyboard_buttons;            // Array of button rows, each represented by an Array of InlineKeyboardButton objects.

  inline void get(nlohmann::json &target_tree) const;
};

inline void inline_keyboard_markup::get(nlohmann::json &tree) const {
  /// Get this reply markup into a json object
  auto &output_keyboard(tree["reply_markup"]["inline_keyboard"]);
  for(auto const &row : keyboard_buttons) {
    output_keyboard.push_back(nlohmann::json());
    auto &output_row(output_keyboard.back());
    for(auto const &it : row) {
      output_row.push_back(nlohmann::json());
      it.get(output_row.back());
    }
  }
}

}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_MARKUP_H_INCLUDED
