#ifndef TELEGRAM_TYPES_REPLY_MARKUP_BASE_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_BASE_H_INCLUDED

namespace telegram {
namespace types {
namespace reply_markup {

template<typename T>
struct base {
  /// CRTP style static polymorphic base class for reply markups
  boost::property_tree::ptree const get_ptree() const;
};

template<typename T>
boost::property_tree::ptree const base<T>::get_ptree() const {
  /// CRTP polymorphic forwarding function to return this reply markup as a property tree
  return static_cast<T>(this)->get_json();
}

}
}
}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_BASE_H_INCLUDED
