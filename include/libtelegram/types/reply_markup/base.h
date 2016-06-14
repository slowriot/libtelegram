#ifndef TELEGRAM_TYPES_REPLY_MARKUP_BASE_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_BASE_H_INCLUDED

namespace telegram {
namespace types {
namespace reply_markup {

template<typename T>
struct base {
  /// CRTP style static polymorphic base class for reply markups
  void get(boost::property_tree::ptree &target_tree) const;
};

template<typename T>
void base<T>::get(boost::property_tree::ptree &tree) const {
  /// CRTP polymorphic forwarding function to get this reply markup into a property tree
  static_cast<T const *>(this)->get(tree);
}

}
}
}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_BASE_H_INCLUDED
