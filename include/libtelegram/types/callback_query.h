#ifndef TELEGRAM_TYPES_CALLBACK_QUERY_H_INCLUDED
#define TELEGRAM_TYPES_CALLBACK_QUERY_H_INCLUDED

#include "optional.h"
#include "user.h"
#include "message.h"

namespace telegram::types {

struct callback_query {
  /// See https://core.telegram.org/bots/api#callbackquery
  std::string id;                                                               // Unique identifier for this query.  (A number, but in a string...?)
  user from;                                                                    // Sender
  std::experimental::optional<message> origin_message;                          // Optional. Message with the callback button that originated the query. Note that message content and message date will not be available if the message is too old.
  std::experimental::optional<std::string> inline_message_id;                   // Optional. Identifier of the message sent via the bot in inline mode, that originated the query.
  std::experimental::optional<std::string> chat_instance;                       // Global identifier, uniquely corresponding to the chat to which the message with the callback button was sent. Useful for high scores in games.
  std::experimental::optional<std::string> data;                                // Optional. Data associated with the callback button. Be aware that a bad client can send arbitrary data in this field.
  std::experimental::optional<std::string> game_short_name;                     // Optional. Short name of a Game to be returned, serves as the unique identifier for the game
  // NOTE: After the user presses an inline button, Telegram clients will display
  // a progress bar until you call answerCallbackQuery. It is, therefore, necessary
  // to react by calling answerCallbackQuery even if no notification to the user
  // is needed (e.g., without specifying any of the optional parameters).

  static callback_query const from_json(nlohmann::json const &tree);
  static callback_query const from_json(nlohmann::json const &tree, std::string const &path);
};

callback_query const callback_query::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return callback_query{tree.at("id"),
                        types::user::from_json(tree, "from"),
                        make_optional<message>(tree, "message"),
                        make_optional<std::string>(tree, "inline_message_id"),
                        make_optional<std::string>(tree, "chat_instance"),
                        make_optional<std::string>(tree, "data"),
                        make_optional<std::string>(tree, "game_short_name")};
}
callback_query const callback_query::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_CALLBACK_QUERY_H_INCLUDED
