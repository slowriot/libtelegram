#ifndef TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED

#include "base.h"

namespace telegram {
namespace types {
namespace reply_markup {

struct reply_keyboard_markup final : base<reply_keyboard_markup> {
  /// See https://core.telegram.org/bots/api/#replykeyboardmarkup
  // TODO

  void get(nlohmann::json &target_tree) const;
};

void reply_keyboard_markup::get(nlohmann::json &tree) const {
  /// Get this reply markup into a json object
  // TODO
}

}
}
}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED
