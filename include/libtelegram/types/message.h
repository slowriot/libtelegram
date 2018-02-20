#ifndef TELEGRAM_TYPES_MESSAGE_H_INCLUDED
#define TELEGRAM_TYPES_MESSAGE_H_INCLUDED

#include "libtelegram/config.h"

#include "helpers/shared.h"
#include "user.h"
#include "chat.h"
#include "audio.h"
#include "document.h"
//#include "game.h"
#include "sticker.h"
#include "video.h"
#include "voice.h"
#include "video_note.h"
#include "contact.h"
#include "location.h"
#include "venue.h"
#include "invoice.h"
#include "successful_payment.h"

namespace telegram::types {

struct message {
  /// See https://core.telegram.org/bots/api#message
  int_fast32_t message_id = 0;                                                  // Unique message identifier
  std::optional<user> from;                                                     // Optional. Sender, can be empty for messages sent to channels
  int_fast32_t date = 0;                                                        // Date the message was sent in Unix time
  types::chat chat;                                                             // Conversation the message belongs to
  std::optional<user> forward_from;                                             // Optional. For forwarded messages, sender of the original message
  std::optional<types::chat> forward_from_chat;                                 // Optional. For messages forwarded from a channel, information about the original channel
  std::optional<int_fast32_t> forward_from_message_id;                          // Optional. For messages forwarded from channels, identifier of the original message in the channel
  std::optional<std::string> forward_signature;                                 // Optional. For messages forwarded from channels, signature of the post author if present
  std::optional<int_fast32_t> forward_date;                                     // Optional. For forwarded messages, date the original message was sent in Unix time
  std::shared_ptr<message> reply_to_message;                                    // Optional. For replies, the original message. Note that the Message object in this field will not contain further reply_to_message fields even if it itself is a reply.
  std::optional<int_fast32_t> edit_date;                                        // Optional. Date the message was last edited in Unix time
  std::optional<std::string> author_signature;                                  // Optional. Signature of the post author for messages in channels
  std::optional<std::string> text;                                              // Optional. For text messages, the actual UTF-8 text of the message, 0-4096 characters.
  //entities                      Array of MessageEntity                          // Optional. For text messages, special entities like usernames, URLs, bot commands, etc. that appear in the text
  std::optional<types::audio> audio;                                            // Optional. Message is an audio file, information about the file
  std::optional<types::document> document;                                      // Optional. Message is a general file, information about the file
  //std::optional<types::game> game;                                              // Optional. Message is a game, information about the game. See https://core.telegram.org/bots/api#games
  std::optional<std::vector<photosize>> photo;                                  // Optional. Message is a photo, available sizes of the photo
  std::optional<types::sticker> sticker;                                        // Optional. Message is a sticker, information about the sticker
  std::optional<types::video> video;                                            // Optional. Message is a video, information about the video
  std::optional<types::voice> voice;                                            // Optional. Message is a voice message, information about the file
  std::optional<types::video_note> video_note;                                  // Optional. Message is a video note, information about the video message
  std::optional<std::string> caption;                                           // Optional. Caption for the document, photo or video, 0-200 characters
  std::optional<types::contact> contact;                                        // Optional. Message is a shared contact, information about the contact
  std::optional<types::location> location;                                      // Optional. Message is a shared location, information about the location
  std::optional<types::venue> venue;                                            // Optional. Message is a venue, information about the venue
  std::optional<std::vector<user>> new_chat_members;                            // Optional. New members that were added to the group or supergroup and information about them (the bot itself may be one of these members)
  std::optional<user> left_chat_member;                                         // Optional. A member was removed from the group, information about them (this member may be the bot itself)
  std::optional<std::string> new_chat_title;                                    // Optional. A chat title was changed to this value
  std::optional<std::vector<photosize>> new_chat_photo;                         // Optional. A chat photo was change to this value
  std::optional<bool> delete_chat_photo;                                        // Optional. Service message: the chat photo was deleted
  std::optional<bool> group_chat_created;                                       // Optional. Service message: the group has been created
  std::optional<bool> supergroup_chat_created;                                  // Optional. Service message: the supergroup has been created. This field can‘t be received in a message coming through updates, because bot can’t be a member of a supergroup when it is created. It can only be found in reply_to_message if someone replies to a very first message in a directly created supergroup.
  std::optional<bool> channel_chat_created;                                     // Optional. Service message: the channel has been created. This field can‘t be received in a message coming through updates, because bot can’t be a member of a channel when it is created. It can only be found in reply_to_message if someone replies to a very first message in a channel.
  std::optional<int_fast64_t> migrate_to_chat_id;                               // Optional. The group has been migrated to a supergroup with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
  std::optional<int_fast64_t> migrate_from_chat_id;                             // Optional. The supergroup has been migrated from a group with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
  std::shared_ptr<message>pinned_message;                                       // Optional. Specified message was pinned. Note that the Message object in this field will not contain further reply_to_message fields even if it is itself a reply.
  std::optional<types::invoice> invoice;                                        // Optional. Message is an invoice for a payment, information about the invoice. See https://core.telegram.org/bots/api#payments
  std::optional<types::successful_payment> successful_payment;                  // Optional. Message is a service message about a successful payment, information about the payment. See https://core.telegram.org/bots/api#payments

  static message const from_json(nlohmann::json const &tree);
  static message const from_json(nlohmann::json const &tree, std::string const &path);
};

inline message const message::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return message{tree.at("message_id"),
                 helpers::make_optional_from_json<user>(tree, "from"),
                 tree.at("date"),
                 types::chat::from_json(tree, "chat"),
                 helpers::make_optional_from_json<user>(tree, "forward_from"),
                 helpers::make_optional_from_json<types::chat>(tree, "forward_from_chat"),
                 helpers::make_optional_from_json<int32_t>(tree, "forward_from_message_id"),
                 helpers::make_optional_from_json<std::string>(tree, "forward_signature"),
                 helpers::make_optional_from_json<int32_t>(tree, "forward_date"),
                 helpers::make_shared_from_json<message>(tree, "reply_to_message"),
                 helpers::make_optional_from_json<int32_t>(tree, "edit_date"),
                 helpers::make_optional_from_json<std::string>(tree, "author_signature"),
                 helpers::make_optional_from_json<std::string>(tree, "text"),
                 //tree.get("entities                                             // Array of MessageEntity
                 helpers::make_optional_from_json<types::audio>(tree, "audio"),
                 helpers::make_optional_from_json<types::document>(tree, "document"),
                 //helpers::make_optional_from_json<types::game>(tree, "game"),
                 helpers::make_optional_vector_from_json<photosize>(tree, "photo"),
                 helpers::make_optional_from_json<types::sticker>(tree, "sticker"),
                 helpers::make_optional_from_json<types::video>(tree, "video"),
                 helpers::make_optional_from_json<types::voice>(tree, "voice"),
                 helpers::make_optional_from_json<types::video_note>(tree, "video_note"),
                 helpers::make_optional_from_json<std::string>(tree, "caption"),
                 helpers::make_optional_from_json<types::contact>(tree, "contact"),
                 helpers::make_optional_from_json<types::location>(tree, "location"),
                 helpers::make_optional_from_json<types::venue>(tree, "venue"),
                 helpers::make_optional_vector_from_json<user>(tree, "new_chat_members"),
                 helpers::make_optional_from_json<user>(tree, "left_chat_member"),
                 helpers::make_optional_from_json<std::string>(tree, "new_chat_title"),
                 helpers::make_optional_vector_from_json<photosize>(tree, "new_chat_photo"),
                 helpers::make_optional_from_json<bool>(tree, "delete_chat_photo"),
                 helpers::make_optional_from_json<bool>(tree, "group_chat_created"),
                 helpers::make_optional_from_json<bool>(tree, "supergroup_chat_created"),
                 helpers::make_optional_from_json<bool>(tree, "channel_chat_created"),
                 helpers::make_optional_from_json<int64_t>(tree, "migrate_to_chat_id"),
                 helpers::make_optional_from_json<int64_t>(tree, "migrate_from_chat_id"),
                 helpers::make_shared_from_json<message>(tree, "pinned_message"),
                 helpers::make_optional_from_json<types::invoice>(tree, "invoice"),
                 helpers::make_optional_from_json<types::successful_payment>(tree, "successful_payment")};
}
inline message const message::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_MESSAGE_H_INCLUDED
