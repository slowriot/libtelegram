#ifndef TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED

#include "base.h"

namespace telegram {
namespace types {
namespace reply_markup {

struct reply_keyboard_markup final : base<reply_keyboard_markup> {
  // TODO
  boost::property_tree::ptree const get_ptree() const;
};

boost::property_tree::ptree const reply_keyboard_markup::get_ptree() const {
  /// Return this reply markup as a property tree
  boost::property_tree::ptree tree;
  // TODO
  return tree;
}

}
}
}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED
