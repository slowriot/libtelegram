#ifndef TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED

#include "libtelegram/config.h"

#include "base.h"
#include "keyboard_button.h"

namespace telegram::types::reply_markup {

struct reply_keyboard_markup final : base<reply_keyboard_markup> {
  /// See https://core.telegram.org/bots/api/#replykeyboardmarkup
  std::vector<std::vector<keyboard_button>> keyboard_buttons;
  enum class resize_keyboard_type : char {                                      // Array of button rows, each represented by an Array of KeyboardButton objects.
    RESIZE,
    NO_RESIZE,
    DEFAULT = NO_RESIZE
  } resize_keyboard = resize_keyboard_type::DEFAULT;                            // Optional. Requests clients to resize the keyboard vertically for optimal fit (e.g., make the keyboard smaller if there are just two rows of buttons). Defaults to false, in which case the custom keyboard is always of the same height as the app's standard keyboard.
  enum class one_time_keyboard_type : char {
    HIDE_AFTER_ONE_USE,
    KEEP_OPEN,
    DEFAULT = KEEP_OPEN
  } one_time_keyboard = one_time_keyboard_type::DEFAULT;                        // Optional. Requests clients to hide the keyboard as soon as it's been used. The keyboard will still be available, but clients will automatically display the usual letter-keyboard in the chat – the user can press a special button in the input field to see the custom keyboard again. Defaults to false.
  enum class selective_type : char {
    SELECTIVE,
    NOT_SELECTIVE,
    DEFAULT = NOT_SELECTIVE
  } selective = selective_type::DEFAULT;                                        // Optional. Use this parameter if you want to show the keyboard to specific users only. Targets: 1) users that are @mentioned in the text of the Message object; 2) if the bot's message is a reply (has reply_to_message_id), sender of the original message.

  inline void get(nlohmann::json &target_tree) const;
};

inline void reply_keyboard_markup::get(nlohmann::json &tree) const {
  /// Get this reply markup into a json object
  auto &output_keyboard(tree["reply_markup"]["keyboard"]);
  for(auto const &row : keyboard_buttons) {
    output_keyboard.push_back(nlohmann::json());
    auto &output_row(output_keyboard.back());
    for(auto const &it : row) {
      output_row.push_back(nlohmann::json());
      it.get(output_row.back());
    }
  }
  if(resize_keyboard != resize_keyboard_type::DEFAULT) {
    switch(resize_keyboard) {
    case resize_keyboard_type::RESIZE:
      tree["reply_markup"]["resize_keyboard"] = true;
      break;
    case resize_keyboard_type::NO_RESIZE:
      tree["reply_markup"]["resize_keyboard"] = false;
      break;
    }
  }
  if(one_time_keyboard != one_time_keyboard_type::DEFAULT) {
    switch(one_time_keyboard) {
    case one_time_keyboard_type::HIDE_AFTER_ONE_USE:
      tree["reply_markup"]["one_time_keyboard"] = true;
      break;
    case one_time_keyboard_type::KEEP_OPEN:
      tree["reply_markup"]["one_time_keyboard"] = false;
      break;
    }
  }
  if(selective != selective_type::DEFAULT) {
    switch(selective) {
    case selective_type::SELECTIVE:
      tree["reply_markup"]["selective"] = true;
      break;
    case selective_type::NOT_SELECTIVE:
      tree["reply_markup"]["selective"] = false;
      break;
    }
  }
}

}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_REPLY_KEYBOARD_MARKUP_H_INCLUDED
