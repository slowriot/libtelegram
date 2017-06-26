#ifndef TELEGRAM_TYPES_CHAT_H_INCLUDED
#define TELEGRAM_TYPES_CHAT_H_INCLUDED

#include <experimental/optional>
#include <json.hpp>
#include "optional.h"

namespace telegram::types {

struct chat {
  /// See https://core.telegram.org/bots/api#chat
  enum class types {
    PRIVATE,
    GROUP,
    SUPERGROUP,
    CHANNEL,
    UNKNOWN
  };

  int_fast64_t id = 0;                                                          // Unique identifier for this chat. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
  types type = types::UNKNOWN;                                                  // Type of chat, can be either “private”, “group”, “supergroup” or “channel”
  std::experimental::optional<std::string> title;                               // Optional. Title, for channels and group chats
  std::experimental::optional<std::string> username;                            // Optional. Username, for private chats, supergroups and channels if available
  std::experimental::optional<std::string> first_name;                          // Optional. First name of the other party in a private chat
  std::experimental::optional<std::string> last_name;                           // Optional. Last name of the other party in a private chat

  static chat const from_json(nlohmann::json const &tree);
  static chat const from_json(nlohmann::json const &tree, std::string const &path);
};

chat const chat::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  types this_type;
  std::string const type_string(tree.at("type").get<std::string>());
  if(       type_string == "private") {
    this_type = types::PRIVATE;
  } else if(type_string == "group") {
    this_type = types::GROUP;
  } else if(type_string == "supergroup") {
    this_type = types::SUPERGROUP;
  } else if(type_string == "channel") {
    this_type = types::CHANNEL;
  } else {
    this_type = types::UNKNOWN;
  }
  return chat{tree.at("id"),
              this_type,
              make_optional<std::string>(tree, "title"),
              make_optional<std::string>(tree, "username"),
              make_optional<std::string>(tree, "first_name"),
              make_optional<std::string>(tree, "last_name")};
}
chat const chat::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_CHAT_H_INCLUDED
