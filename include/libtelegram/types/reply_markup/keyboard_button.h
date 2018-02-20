#ifndef TELEGRAM_TYPES_REPLY_MARKUP_KEYBOARD_BUTTON_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_KEYBOARD_BUTTON_H_INCLUDED

#include "libtelegram/config.h"

namespace telegram::types::reply_markup {

struct keyboard_button {
  std::string text;                                                             // Text of the button. If none of the optional fields are used, it will be sent to the bot as a message when the button is pressed
  enum class option_type : char {
    NONE,
    REQUEST_CONTACT,                                                            // Optional. If True, the user's phone number will be sent as a contact when the button is pressed. Available in private chats only
    REQUEST_LOCATION,                                                           // Optional. If True, the user's current location will be sent when the button is pressed. Available in private chats only
    DEFAULT = NONE
  } option = option_type::DEFAULT;

  inline keyboard_button(std::string const &this_text, option_type this_option = option_type::DEFAULT);

  inline void get(nlohmann::json &tree) const;
};

inline keyboard_button::keyboard_button(std::string const &this_text,
                                        option_type this_option)
  : text(this_text),
    option(this_option) {
  /// Constructor
}

inline void keyboard_button::get(nlohmann::json &tree) const {
  /// Get this keyboard button into a json object (presuming the target is a key in a keyboard)
  tree["text"] = text;
  if(option != option_type::DEFAULT) {
    switch(option) {
    case option_type::REQUEST_CONTACT:
      tree["request_contact"] = true;
      break;
    case option_type::REQUEST_LOCATION:
      tree["request_location"] = true;
      break;
    default:
      break;
    }
  }
}

}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_KEYBOARD_BUTTON_H_INCLUDED
