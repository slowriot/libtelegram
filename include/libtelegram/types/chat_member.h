#ifndef TELEGRAM_TYPES_CHAT_MEMBER_H_INCLUDED
#define TELEGRAM_TYPES_CHAT_MEMBER_H_INCLUDED

#include "helpers/optional.h"

namespace telegram::types {

struct chat_member {
  /// See https://core.telegram.org/bots/api#chatmember
  enum class status_type {
    CREATOR,
    ADMINISTRATOR,
    MEMBER,
    RESTRICTED,
    LEFT,
    KICKED,
    UNKNOWN
  };

  user chat_user;                                                               // Information about the user
  status_type status = status_type::UNKNOWN;                                    // The member's status in the chat. Can be “creator”, “administrator”, “member”, “restricted”, “left” or “kicked”
  std::optional<int_fast32_t> until_date = 0;                                   // Optional. Restictred and kicked only. Date when restrictions will be lifted for this user, unix time
  std::optional<bool> can_be_edited             = false;                        // Optional. Administrators only. True, if the bot is allowed to edit administrator privileges of that user
  std::optional<bool> can_change_info           = false;                        // Optional. Administrators only. True, if the administrator can change the chat title, photo and other settings
  std::optional<bool> can_post_messages         = false;                        // Optional. Administrators only. True, if the administrator can post in the channel, channels only
  std::optional<bool> can_edit_messages         = false;                        // Optional. Administrators only. True, if the administrator can edit messages of other users, channels only
  std::optional<bool> can_delete_messages       = false;                        // Optional. Administrators only. True, if the administrator can delete messages of other users
  std::optional<bool> can_invite_users          = false;                        // Optional. Administrators only. True, if the administrator can invite new users to the chat
  std::optional<bool> can_restrict_members      = false;                        // Optional. Administrators only. True, if the administrator can restrict, ban or unban chat members
  std::optional<bool> can_pin_messages          = false;                        // Optional. Administrators only. True, if the administrator can pin messages, supergroups only
  std::optional<bool> can_promote_members       = false;                        // Optional. Administrators only. True, if the administrator can add new administrators with a subset of his own privileges or demote administrators that he has promoted, directly or indirectly (promoted by administrators that were appointed by the user)
  std::optional<bool> can_send_messages         = false;                        // Optional. Restricted only. True, if the user can send text messages, contacts, locations and venues
  std::optional<bool> can_send_media_messages   = false;                        // Optional. Restricted only. True, if the user can send audios, documents, photos, videos, video notes and voice notes, implies can_send_messages
  std::optional<bool> can_send_other_messages   = false;                        // Optional. Restricted only. True, if the user can send animations, games, stickers and use inline bots, implies can_send_media_messages
  std::optional<bool> can_add_web_page_previews = false;                        // Optional. Restricted only. True, if user may add web page previews to his messages, implies can_send_media_messages

  static chat_member const from_json(nlohmann::json const &tree);
  static chat_member const from_json(nlohmann::json const &tree, std::string const &path);
};

inline chat_member const chat_member::from_json(nlohmann::json const &tree) {
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
  } else if(status_string == "restricted") {
    this_status = status_type::RESTRICTED;
  } else if(status_string == "left") {
    this_status = status_type::LEFT;
  } else if(status_string == "kicked") {
    this_status = status_type::KICKED;
  } else {
    this_status = status_type::UNKNOWN;
  }
  return chat_member{types::user::from_json(tree, "user"),
                     this_status,
                     helpers::make_optional_from_json<int_fast32_t>(tree, "until_date"),
                     helpers::make_optional_from_json<bool>(tree, "can_be_edited"),
                     helpers::make_optional_from_json<bool>(tree, "can_change_info"),
                     helpers::make_optional_from_json<bool>(tree, "can_post_messages"),
                     helpers::make_optional_from_json<bool>(tree, "can_edit_messages"),
                     helpers::make_optional_from_json<bool>(tree, "can_delete_messages"),
                     helpers::make_optional_from_json<bool>(tree, "can_invite_users"),
                     helpers::make_optional_from_json<bool>(tree, "can_restrict_members"),
                     helpers::make_optional_from_json<bool>(tree, "can_pin_messages"),
                     helpers::make_optional_from_json<bool>(tree, "can_promote_members"),
                     helpers::make_optional_from_json<bool>(tree, "can_send_messages"),
                     helpers::make_optional_from_json<bool>(tree, "can_send_media_messages"),
                     helpers::make_optional_from_json<bool>(tree, "can_send_other_messages"),
                     helpers::make_optional_from_json<bool>(tree, "can_add_web_page_previews")};
}
inline chat_member const chat_member::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_CHAT_MEMBER_H_INCLUDED
