#ifndef TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_MARKUP_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_MARKUP_H_INCLUDED

#include "base.h"

namespace telegram {
namespace types {
namespace reply_markup {

struct inline_keyboard_markup final : base<inline_keyboard_markup> {
  /// See https://core.telegram.org/bots/api/#inlinekeyboardmarkup
  // TODO

  void get(boost::property_tree::ptree &target_tree) const;
};

void inline_keyboard_markup::get(boost::property_tree::ptree &tree) const {
  /// Get this reply markup into a property tree
  // TODO
}

}
}
}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_MARKUP_H_INCLUDED
