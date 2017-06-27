#ifndef TELEGRAM_TYPES_CHAT_MEMBER_H_INCLUDED
#define TELEGRAM_TYPES_CHAT_MEMBER_H_INCLUDED

namespace telegram::types {

struct chat_member {
  /// See https://core.telegram.org/bots/api#chatmember
  enum class status_type {
    CREATOR,
    ADMINISTRATOR,
    MEMBER,
    LEFT,
    KICKED,
    UNKNOWN
  };

  user chat_user;                                                               // Information about the user
  status_type status = status_type::UNKNOWN;                                    // The member's status in the chat. Can be “creator”, “administrator”, “member”, “left” or “kicked”

  static chat_member const from_json(nlohmann::json const &tree);
  static chat_member const from_json(nlohmann::json const &tree, std::string const &path);
};

chat_member const chat_member::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  status_type this_status;
  std::string const status_string(tree.at("type").get<std::string>());
  if(       status_string == "creator") {
    this_status = status_type::CREATOR;
  } else if(status_string == "administrator") {
    this_status = status_type::ADMINISTRATOR;
  } else if(status_string == "member") {
    this_status = status_type::MEMBER;
  } else if(status_string == "left") {
    this_status = status_type::LEFT;
  } else if(status_string == "kicked") {
    this_status = status_type::KICKED;
  } else {
    this_status = status_type::UNKNOWN;
  }
  return chat_member{types::user::from_json(tree, "user"),
                     this_status};
}
chat_member const chat_member::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_CHAT_MEMBER_H_INCLUDED
