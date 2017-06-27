#ifndef TELEGRAM_TYPES_MESSAGE_H_INCLUDED
#define TELEGRAM_TYPES_MESSAGE_H_INCLUDED

#include "optional.h"
#include "photosize.h"
#include "user.h"
#include "chat.h"

namespace telegram::types {

struct message {
  /// See https://core.telegram.org/bots/api#message
  int_fast32_t message_id = 0;                                                  // Unique message identifier
  std::experimental::optional<user> from;                                       // Optional. Sender, can be empty for messages sent to channels
  int_fast32_t date = 0;                                                        // Date the message was sent in Unix time
  types::chat chat;                                                             // Conversation the message belongs to
  std::experimental::optional<user> forward_from;                               // Optional. For forwarded messages, sender of the original message
  std::experimental::optional<types::chat> forward_from_chat;                   // Optional. For messages forwarded from a channel, information about the original channel
  std::experimental::optional<int_fast32_t> forward_date;                       // Optional. For forwarded messages, date the original message was sent in Unix time
  //std::experimental::optional<message> reply_to_message;                        // Optional. For replies, the original message. Note that the Message object in this field will not contain further reply_to_message fields even if it itself is a reply.
  std::experimental::optional<int_fast32_t> edit_date;                          // Optional. Date the message was last edited in Unix time
  std::experimental::optional<std::string> text;                                // Optional. For text messages, the actual UTF-8 text of the message, 0-4096 characters.
  //entities                      Array of MessageEntity                          // Optional. For text messages, special entities like usernames, URLs, bot commands, etc. that appear in the text
  //std::experimental::optional<types::audio> audio;                              // Optional. Message is an audio file, information about the file
  //std::experimental::optional<types::document> document;                        // Optional. Message is a general file, information about the file
  std::experimental::optional<std::vector<photosize>> photo;                    // Optional. Message is a photo, available sizes of the photo
  //std::experimental::optional<types::sticker> sticker;                          // Optional. Message is a sticker, information about the sticker
  //std::experimental::optional<types::video> video;                              // Optional. Message is a video, information about the video
  //std::experimental::optional<types::voice> voice;                              // Optional. Message is a voice message, information about the file
  std::experimental::optional<std::string> caption;                             // Optional. Caption for the document, photo or video, 0-200 characters
  //std::experimental::optional<types::contact> contact;                          // Optional. Message is a shared contact, information about the contact
  //std::experimental::optional<types::location> location;                        // Optional. Message is a shared location, information about the location
  //std::experimental::optional<types::venue> venue;                              // Optional. Message is a venue, information about the venue
  std::experimental::optional<user> new_chat_member;                            // Optional. A new member was added to the group, information about them (this member may be the bot itself)
  std::experimental::optional<user> left_chat_member;                           // Optional. A member was removed from the group, information about them (this member may be the bot itself)
  std::experimental::optional<std::string> new_chat_title;                      // Optional. A chat title was changed to this value
  std::experimental::optional<std::vector<photosize>> new_chat_photo;           // Optional. A chat photo was change to this value
  std::experimental::optional<bool> delete_chat_photo;                          // Optional. Service message: the chat photo was deleted
  std::experimental::optional<bool> group_chat_created;                         // Optional. Service message: the group has been created
  std::experimental::optional<bool> supergroup_chat_created;                    // Optional. Service message: the supergroup has been created. This field can‘t be received in a message coming through updates, because bot can’t be a member of a supergroup when it is created. It can only be found in reply_to_message if someone replies to a very first message in a directly created supergroup.
  std::experimental::optional<bool> channel_chat_created;                       // Optional. Service message: the channel has been created. This field can‘t be received in a message coming through updates, because bot can’t be a member of a channel when it is created. It can only be found in reply_to_message if someone replies to a very first message in a channel.
  std::experimental::optional<int_fast64_t> migrate_to_chat_id;                 // Optional. The group has been migrated to a supergroup with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
  std::experimental::optional<int_fast64_t> migrate_from_chat_id;               // Optional. The supergroup has been migrated from a group with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
  //std::experimental::optional<message>pinned_message;                           // Optional. Specified message was pinned. Note that the Message object in this field will not contain further reply_to_message fields even if it is itself a reply.

  static message const from_json(nlohmann::json const &tree);
  static message const from_json(nlohmann::json const &tree, std::string const &path);
};

message const message::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return message{tree.at("message_id"),
                 make_optional<user>(tree, "from"),
                 tree.at("date"),
                 types::chat::from_json(tree, "chat"),
                 make_optional<user>(tree, "forward_from"),
                 make_optional<types::chat>(tree, "forward_from_chat"),
                 make_optional<int_fast32_t>(tree, "forward_date"),
                 //message::from_json(tree, "reply_to_message"),
                 make_optional<int_fast32_t>(tree, "edit_date"),
                 make_optional<std::string>(tree, "text"),
                 //tree.get("entities                                             // Array of MessageEntity
                 //audio::from_json(tree, "audio"),
                 //document::from_json(tree, "document"),
                 make_optional_vector<photosize>(tree, "photo"),
                 //sticker::from_json(tree, "sticker"),
                 //video::from_json(tree, "video"),
                 //voice::from_json(tree, "voice"),
                 make_optional<std::string>(tree, "caption"),
                 //contact::from_json(tree, "contact"),
                 //location::from_json(tree, "location"),
                 //venue::from_json(tree, "venue"),
                 make_optional<user>(tree, "new_chat_member"),
                 make_optional<user>(tree, "left_chat_member"),
                 make_optional<std::string>(tree, "new_chat_title"),
                 make_optional_vector<photosize>(tree, "new_chat_photo"),
                 make_optional<bool>(tree, "delete_chat_photo"),
                 make_optional<bool>(tree, "group_chat_created"),
                 make_optional<bool>(tree, "supergroup_chat_created"),
                 make_optional<bool>(tree, "channel_chat_created"),
                 make_optional<int_fast64_t>(tree, "migrate_to_chat_id"),
                 make_optional<int_fast64_t>(tree, "migrate_from_chat_id")};
                 //make_optional<message>(tree, "pinned_message"};
}
message const message::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_MESSAGE_H_INCLUDED
