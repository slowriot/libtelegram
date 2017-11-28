#ifndef TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_BUTTON_H_INCLUDED
#define TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_BUTTON_H_INCLUDED

#include <optional>
#include <variant>
#include "libtelegram/types/callback_game.h"

namespace telegram::types::reply_markup {

struct inline_keyboard_button {
  std::string text;                                                             // Label text on the button.
  enum class type : char {
    URL,
    CALLBACK,
    SWITCH,
    SWITCH_CURRENT,
    GAME,
    PAY
  };

  std::optional<std::string> url;                                               // Optional. HTTP url to be opened when button is pressed.
  std::optional<std::string> callback_data;                                     // Optional. Data to be sent in a callback query to the bot when button is pressed, 1-64 bytes.
  std::optional<std::string> switch_inline_query;                               // Optional. If set, pressing the button will prompt the user to select one of their chats, open that chat and insert the bot‘s username and the specified inline query in the input field. Can be empty, in which case just the bot’s username will be inserted.  Note: This offers an easy way for users to start using your bot in inline mode when they are currently in a private chat with it. Especially useful when combined with switch_pm… actions – in this case the user will be automatically returned to the chat they switched from, skipping the chat selection screen.
  std::optional<std::string> switch_inline_query_current_chat;                  // Optional. If set, pressing the button will insert the bot‘s username and the specified inline query in the current chat's input field. Can be empty, in which case only the bot’s username will be inserted.  This offers a quick way for the user to open your bot in inline mode in the same chat – good for selecting something from multiple options.
  std::optional<types::callback_game> callback_game;                            // Optional. Description of the game that will be launched when the user presses the button.  NOTE: This type of button must always be the first button in the first row.
  bool pay = false;                                                             // Optional. Specify true, to send a Pay button.  NOTE: This type of button must always be the first button in the first row.

  inline inline_keyboard_button(std::string const &this_text,
                                type this_type,
                                std::variant<std::string, types::callback_game, bool> const &content = std::string{});

  inline void get(nlohmann::json &tree) const;
};

inline inline_keyboard_button::inline_keyboard_button(std::string const &this_text,
                                                      type this_type,
                                                      std::variant<std::string, types::callback_game, bool> const &content)
  : text(this_text) {
  /// Constructor
  switch(this_type) {
  case type::URL:
    url = std::get<std::string>(content);
    break;
  case type::CALLBACK:
    callback_data = std::get<std::string>(content);
    break;
  case type::SWITCH:
    switch_inline_query = std::get<std::string>(content);
    break;
  case type::SWITCH_CURRENT:
    switch_inline_query_current_chat = std::get<std::string>(content);
    break;
  case type::GAME:
    callback_game = std::get<types::callback_game>(content);
    break;
  case type::PAY:
    pay = true;
    break;
  }
}

inline void inline_keyboard_button::get(nlohmann::json &tree) const {
  /// Get this keyboard button into a json object (presuming the target is a key in a keyboard)
  tree["text"] = text;
  if(url) {
    tree["url"] = *url;
  }
  if(callback_data) {
    tree["callback_data"] = *callback_data;
  }
  if(switch_inline_query) {
    tree["switch_inline_query"] = *switch_inline_query;
  }
  if(switch_inline_query_current_chat) {
    tree["switch_inline_query_current_chat"] = *switch_inline_query_current_chat;
  }
  if(callback_game) {
    callback_game->get(tree["callback_game"]);
  }
  if(pay) {
    tree["pay"] = true;
  }
}

}

#endif // TELEGRAM_TYPES_REPLY_MARKUP_INLINE_KEYBOARD_BUTTON_H_INCLUDED
