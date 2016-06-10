#ifndef TELEGRAM_TYPES_MESSAGE_H_INCLUDED
#define TELEGRAM_TYPES_MESSAGE_H_INCLUDED

#include "optional.h"
#include "photosize.h"
#include "user.h"

namespace telegram {
namespace types {

struct message {
  /// See https://core.telegram.org/bots/api#message
  int_fast32_t message_id = 0;                                                  // Unique message identifier
  std::experimental::optional<user> from;                                       // Optional. Sender, can be empty for messages sent to channels
  int_fast32_t date = 0;                                                        // Date the message was sent in Unix time
  types::chat chat;                                                             // Conversation the message belongs to
  std::experimental::optional<user> forward_from;                               // Optional. For forwarded messages, sender of the original message
  std::experimental::optional<types::chat> forward_from_chat;                   // Optional. For messages forwarded from a channel, information about the original channel
  int_fast32_t forward_date = 0;                                                // Optional. For forwarded messages, date the original message was sent in Unix time
  //message reply_to_message;                                                     // Optional. For replies, the original message. Note that the Message object in this field will not contain further reply_to_message fields even if it itself is a reply.
  int_fast32_t edit_date = 0;                                                   // Optional. Date the message was last edited in Unix time
  std::string text;                                                             // Optional. For text messages, the actual UTF-8 text of the message, 0-4096 characters.
  //entities                      Array of MessageEntity                          // Optional. For text messages, special entities like usernames, URLs, bot commands, etc. that appear in the text
  //std::experimental::optional<types::audio> audio;                              // Optional. Message is an audio file, information about the file
  //std::experimental::optional<types::document> document;                        // Optional. Message is a general file, information about the file
  std::experimental::optional<std::vector<photosize>> photo;                    // Optional. Message is a photo, available sizes of the photo
  //std::experimental::optional<types::sticker> sticker;                          // Optional. Message is a sticker, information about the sticker
  //std::experimental::optional<types::video> video;                              // Optional. Message is a video, information about the video
  //std::experimental::optional<types::voice> voice;                              // Optional. Message is a voice message, information about the file
  std::string caption;                                                          // Optional. Caption for the document, photo or video, 0-200 characters
  //std::experimental::optional<types::contact> contact;                          // Optional. Message is a shared contact, information about the contact
  //std::experimental::optional<types::location> location;                        // Optional. Message is a shared location, information about the location
  //std::experimental::optional<types::venue> venue;                              // Optional. Message is a venue, information about the venue
  //user new_chat_member;                                                         // Optional. A new member was added to the group, information about them (this member may be the bot itself)
  //user left_chat_member;                                                        // Optional. A member was removed from the group, information about them (this member may be the bot itself)
  std::string new_chat_title;                                                   // Optional. A chat title was changed to this value
  std::experimental::optional<std::vector<photosize>> new_chat_photo;           // Optional. A chat photo was change to this value
  bool delete_chat_photo = false;                                               // Optional. Service message: the chat photo was deleted
  bool group_chat_created = false;                                              // Optional. Service message: the group has been created
  bool supergroup_chat_created = false;                                         // Optional. Service message: the supergroup has been created. This field can‘t be received in a message coming through updates, because bot can’t be a member of a supergroup when it is created. It can only be found in reply_to_message if someone replies to a very first message in a directly created supergroup.
  bool channel_chat_created = false;                                            // Optional. Service message: the channel has been created. This field can‘t be received in a message coming through updates, because bot can’t be a member of a channel when it is created. It can only be found in reply_to_message if someone replies to a very first message in a channel.
  int_fast64_t migrate_to_chat_id = 0;                                          // Optional. The group has been migrated to a supergroup with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
  int_fast64_t migrate_from_chat_id = 0;                                        // Optional. The supergroup has been migrated from a group with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
  //std::experimental::optional<message>pinned_message;                           // Optional. Specified message was pinned. Note that the Message object in this field will not contain further reply_to_message fields even if it is itself a reply.

  static message const from_ptree(boost::property_tree::ptree const &tree);
  static message const from_ptree(boost::property_tree::ptree const &tree, std::string const &path);
};

message const message::from_ptree(boost::property_tree::ptree const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws boost::property_tree::ptree_bad_path
  return message{tree.get("message_id", 0),
                 make_optional<user>(tree, "from"),
                 tree.get("date", 0),
                 types::chat::from_ptree(tree, "chat"),
                 make_optional<user>(tree, "forward_from"),
                 make_optional<types::chat>(tree, "forward_from_chat"),
                 tree.get("forward_date", 0),
                 //message::from_ptree(tree, "reply_to_message"),
                 tree.get("edit_date", 0),
                 tree.get("text", ""),
                 //tree.get("entities                                             // Array of MessageEntity
                 //audio::from_ptree(tree, "audio"),
                 //document::from_ptree(tree, "document"),
                 make_optional_vector<photosize>(tree, "photo"),
                 //sticker::from_ptree(tree, "sticker"),
                 //video::from_ptree(tree, "video"),
                 //voice::from_ptree(tree, "voice"),
                 tree.get("caption", ""),
                 //contact::from_ptree(tree, "contact"),
                 //location::from_ptree(tree, "location"),
                 //venue::from_ptree(tree, "venue"),
                 //user::from_ptree(tree, "new_chat_member"),
                 //user::from_ptree(tree, "left_chat_member"),
                 tree.get("new_chat_title", ""),
                 make_optional_vector<photosize>(tree, "new_chat_photo"),
                 tree.get("delete_chat_photo",       "") == "True" ? true : false,
                 tree.get("group_chat_created",      "") == "True" ? true : false,
                 tree.get("supergroup_chat_created", "") == "True" ? true : false,
                 tree.get("channel_chat_created",    "") == "True" ? true : false,
                 tree.get<int_fast64_t>("migrate_to_chat_id", 0),
                 tree.get<int_fast64_t>("migrate_from_chat_id", 0)};
                 //make_optional<message>(tree, "pinned_message"};
}
message const message::from_ptree(boost::property_tree::ptree const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws boost::property_tree::ptree_bad_path
  return from_ptree(tree.get_child(path));
}

}
}

#endif // TELEGRAM_TYPES_MESSAGE_H_INCLUDED
