#ifndef TELEGRAM_TYPES_CALLBACK_QUERY_H_INCLUDED
#define TELEGRAM_TYPES_CALLBACK_QUERY_H_INCLUDED

#include "message.h"

namespace telegram::types {

struct callback_query {
  /// See https://core.telegram.org/bots/api#callbackquery
  std::string id;                                                               // Unique identifier for this query.  (A number, but in a string...?)
  user from;                                                                    // Sender
  std::optional<message> origin_message;                                        // Optional. Message with the callback button that originated the query. Note that message content and message date will not be available if the message is too old.
  std::optional<std::string> inline_message_id;                                 // Optional. Identifier of the message sent via the bot in inline mode, that originated the query.
  std::optional<std::string> chat_instance;                                     // Global identifier, uniquely corresponding to the chat to which the message with the callback button was sent. Useful for high scores in games.
  std::optional<std::string> data;                                              // Optional. Data associated with the callback button. Be aware that a bad client can send arbitrary data in this field.
  std::optional<std::string> game_short_name;                                   // Optional. Short name of a Game to be returned, serves as the unique identifier for the game
  // NOTE: After the user presses an inline button, Telegram clients will display
  // a progress bar until you call answerCallbackQuery. It is, therefore, necessary
  // to react by calling answerCallbackQuery even if no notification to the user
  // is needed (e.g., without specifying any of the optional parameters).

  static callback_query const from_json(nlohmann::json const &tree);
  static callback_query const from_json(nlohmann::json const &tree, std::string const &path);
};

inline callback_query const callback_query::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return callback_query{tree.at("id"),
                        user::from_json(tree, "from"),
                        helpers::make_optional_from_json<message>(tree, "message"),
                        helpers::make_optional_from_json<std::string>(tree, "inline_message_id"),
                        helpers::make_optional_from_json<std::string>(tree, "chat_instance"),
                        helpers::make_optional_from_json<std::string>(tree, "data"),
                        helpers::make_optional_from_json<std::string>(tree, "game_short_name")};
}
inline callback_query const callback_query::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_CALLBACK_QUERY_H_INCLUDED
