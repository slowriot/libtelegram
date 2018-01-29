#ifndef TELEGRAM_SENDER_H_INCLUDED
#define TELEGRAM_SENDER_H_INCLUDED

#include <string>
#include <variant>
#include <boost/iostreams/stream.hpp>
#define URDL_HEADER_ONLY 1
#ifdef LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING
  #warning "SSL is disabled for outgoing messages - that is such a bad idea."
  #define URDL_DISABLE_SSL 1
  #define LIBTELEGRAM_OUTGOING_PROTO "http"
#else
  #define LIBTELEGRAM_OUTGOING_PROTO "https"
#endif // LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING
#include <urdl/istream.hpp>
#include <json.hpp>
#include "types/message.h"
#include "types/file.h"
#include "types/chat_member.h"
#include "types/reply_markup/reply_markup.h"

// helper for chat id template verification
#define VERIFY_CHAT_ID static_assert(std::is_same<Tchat_id, int_fast64_t>::value || std::is_same<Tchat_id, std::string const&>::value, "Chat ID must be either a 64bit integer or a string");

namespace telegram {

class sender {
  std::string token;                                                            // the bot's Telegram authentication token
  std::string endpoint;                                                         // the web endpoint requests are sent to
  std::string user_agent;                                                       // the user agent the bot should identify itself with

  urdl::option_set urdl_global_options;                                         // global options shared by all outgoing web requests

public:
  enum class parse_mode : char {                                                // see https://core.telegram.org/bots/api#markdown-style
    NONE,
    MARKDOWN,
    HTML,
    UNCHANGED,                                                                  // used when editing messages
    DEFAULT = NONE
  };
  enum class web_preview_mode : char {                                          // whether or not to allow the web preview for links, see https://core.telegram.org/bots/api#sendmessage
    DISABLE,
    ENABLE,
    UNCHANGED,                                                                  // used when editing messages
    DEFAULT = ENABLE
  };
  enum class notification_mode : char {                                         // whether to send the message silently, see https://core.telegram.org/bots/api#sendmessage
    DISABLE,
    ENABLE,
    UNCHANGED,                                                                  // used when editing messages
    DEFAULT = ENABLE
  };
  enum class chat_action_type : char {                                          // chat actions, see https://core.telegram.org/bots/api#sendchataction
    TYPING,                                                                     // typing for text messages
    UPLOAD_PHOTO,                                                               // upload_photo for photos
    RECORD_VIDEO,                                                               // record_video for videos
    UPLOAD_VIDEO,                                                               // upload_video for videos
    RECORD_AUDIO,                                                               // record_audio for audio files
    UPLOAD_AUDIO,                                                               // upload_audio for audio files
    UPLOAD_DOCUMENT,                                                            // upload_document for general files
    FIND_LOCATION                                                               // find_location for location data
  };
  static int_fast32_t constexpr const reply_to_message_id_none = -1;
  static int_fast32_t constexpr const live_location_period_none = -1;
  static int_fast32_t constexpr const message_length_limit = 4096;              // see https://core.telegram.org/method/messages.sendMessage

  // TODO: add message sending stream class
  // TODO: add statistics on bytes sent and received

  inline sender(std::string const &token, std::string const &user_agent = "LibTelegram");

  inline nlohmann::json send_json(std::string const &method,
                                  nlohmann::json const &tree = {},
                                  unsigned int poll_timeout = 30);
  template<typename T>
  inline std::optional<T> send_json_and_parse(std::string const &method,
                                              nlohmann::json const &tree = {});
  template<typename T>
  inline std::optional<std::vector<T>> send_json_and_parse_vector(std::string const &method,
                                                                  nlohmann::json const &tree = {});
  inline bool send_json_and_get_bool(std::string const &method,
                                     nlohmann::json const &tree = {});

  inline std::optional<types::user> const get_me();

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<types::message> send_message(Tchat_id chat_id,
                                                    std::string const &text,
                                                    int_fast32_t reply_to_message_id = reply_to_message_id_none,
                                                    parse_mode parse = parse_mode::DEFAULT,
                                                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                                    notification_mode notification = notification_mode::DEFAULT,
                                                    std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                               types::reply_markup::reply_keyboard_markup,
                                                                               types::reply_markup::reply_keyboard_remove,
                                                                               types::reply_markup::force_reply>> reply_markup = std::nullopt);
  template<typename Tchat_id = int_fast64_t>
  inline std::optional<types::message> send_message(Tchat_id chat_id,
                                                    std::string const &text,
                                                    std::variant<types::reply_markup::inline_keyboard_markup,
                                                                 types::reply_markup::reply_keyboard_markup,
                                                                 types::reply_markup::reply_keyboard_remove,
                                                                 types::reply_markup::force_reply> reply_markup,
                                                    int_fast32_t reply_to_message_id = reply_to_message_id_none,
                                                    parse_mode parse = parse_mode::DEFAULT,
                                                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                                    notification_mode notification = notification_mode::DEFAULT);

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<types::message> forward_message(Tchat_id chat_id,
                                                       int_fast64_t from_chat_id,
                                                       int_fast32_t message_id,
                                                       notification_mode notification = notification_mode::DEFAULT);

  // updating messages
  template<typename Tchat_id = int_fast64_t>
  inline bool edit_message_text(std::string const &text,
                                Tchat_id chat_id,
                                int_fast32_t message_id,
                                parse_mode parse = parse_mode::DEFAULT,
                                web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_remove,
                                                           types::reply_markup::force_reply>> reply_markup = std::nullopt);
  inline bool edit_message_text(std::string const &text,
                                std::string const &inline_message_id,
                                parse_mode parse = parse_mode::DEFAULT,
                                web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_remove,
                                                           types::reply_markup::force_reply>> reply_markup = std::nullopt);

  template<typename Tchat_id = int_fast64_t>
  inline bool edit_message_caption(Tchat_id chat_id,
                                   int_fast32_t message_id,
                                   std::string const &caption,
                                   std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                              types::reply_markup::reply_keyboard_markup,
                                                              types::reply_markup::reply_keyboard_remove,
                                                              types::reply_markup::force_reply>> reply_markup = std::nullopt);
  inline bool edit_message_caption(std::string const &inline_message_id,
                                   std::string const &caption,
                                   std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                              types::reply_markup::reply_keyboard_markup,
                                                              types::reply_markup::reply_keyboard_remove,
                                                              types::reply_markup::force_reply>> reply_markup = std::nullopt);

  template<typename Tchat_id = int_fast64_t>
  inline bool edit_message_reply_markup(Tchat_id chat_id,
                                        int_fast32_t message_id,
                                        std::variant<types::reply_markup::inline_keyboard_markup,
                                                     types::reply_markup::reply_keyboard_markup,
                                                     types::reply_markup::reply_keyboard_remove,
                                                     types::reply_markup::force_reply> reply_markup);
  inline bool edit_message_reply_markup(std::string const &inline_message_id,
                                        std::variant<types::reply_markup::inline_keyboard_markup,
                                                     types::reply_markup::reply_keyboard_markup,
                                                     types::reply_markup::reply_keyboard_remove,
                                                     types::reply_markup::force_reply> reply_markup);
  // TODO: deleteMessage

  // TODO: sendPhoto
  // TODO: sendAudio
  // TODO: sendDocument
  // TODO: sendSticker
  // TODO: sendVideo
  // TODO: sendVoice
  // TODO: sendVideoNote
  // TODO: sendMediaGroup

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<types::message> send_location(Tchat_id chat_id,
                                                     types::location const &location,
                                                     int_fast32_t live_period = live_location_period_none,
                                                     bool disable_notification = false,
                                                     int_fast32_t reply_to_message_id = reply_to_message_id_none,
                                                     std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                                types::reply_markup::reply_keyboard_markup,
                                                                                types::reply_markup::reply_keyboard_remove,
                                                                                types::reply_markup::force_reply>> reply_markup = std::nullopt);

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<types::message> edit_message_live_location(Tchat_id chat_id,
                                                                  int_fast32_t message_id,
                                                                  types::location const &location,
                                                                  std::optional<types::reply_markup::inline_keyboard_markup> reply_markup = std::nullopt);
  inline std::optional<types::message> edit_message_live_location(std::string const &inline_message_id,
                                                                  types::location const &location,
                                                                  std::optional<types::reply_markup::inline_keyboard_markup> reply_markup = std::nullopt);

  // TODO: stopMessageLiveLocation
  // TODO: sendVenue
  // TODO: sendContact

  template<typename Tchat_id = int_fast64_t>
  inline bool send_chat_action(Tchat_id chat_id,
                               chat_action_type action = chat_action_type::TYPING);

  // TODO: getUserProfilePhotos

  inline std::optional<types::file> get_file(std::string const &file_id);

  template<typename Tchat_id = int_fast64_t>
  inline bool kick_chat_member(Tchat_id chat_id,
                               int_fast32_t user_id,
                               int_fast32_t until_date = 0);

  template<typename Tchat_id = int_fast64_t>
  inline bool unban_chat_member(Tchat_id chat_id,
                                int_fast32_t user_id);

  template<typename Tchat_id = int_fast64_t>
  inline bool restrict_chat_member(Tchat_id chat_id,
                                   int_fast32_t user_id,
                                   int_fast32_t until_date,
                                   bool can_send_messages = false,
                                   bool can_send_media_messages = false,
                                   bool can_send_other_messages = false,
                                   bool can_add_web_page_previews = false);

  template<typename Tchat_id = int_fast64_t>
  inline bool promote_chat_member(Tchat_id chat_id,
                                  int_fast32_t user_id,
                                  bool can_change_info = false,
                                  bool can_post_messages = false,
                                  bool can_edit_messages = false,
                                  bool can_delete_messages = false,
                                  bool can_invite_users = false,
                                  bool can_restrict_members = false,
                                  bool can_pin_messages = false,
                                  bool can_promote_members = false);

  // TODO: exportChatInviteLink
  // TODO: setChatPhoto

  template<typename Tchat_id = int_fast64_t>
  inline bool delete_chat_photo(Tchat_id chat_id);

  template<typename Tchat_id = int_fast64_t>
  inline bool set_chat_title(Tchat_id chat_id,
                             std::string const &title);

  template<typename Tchat_id = int_fast64_t>
  inline bool set_chat_description(Tchat_id chat_id,
                                   std::string const &description);

  template<typename Tchat_id = int_fast64_t>
  inline bool pin_chat_message(Tchat_id chat_id,
                               int_fast32_t message_id,
                               bool disable_notification = false);

  template<typename Tchat_id = int_fast64_t>
  inline bool unpin_chat_message(Tchat_id chat_id);

  template<typename Tchat_id = int_fast64_t>
  inline bool leave_chat(Tchat_id chat_id);

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<types::chat> get_chat(Tchat_id chat_id);

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<std::vector<types::chat_member>> get_chat_administrators(Tchat_id chat_id);

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<int_fast32_t> get_chat_members_count(Tchat_id chat_id);

  template<typename Tchat_id = int_fast64_t>
  inline std::optional<types::chat_member> get_chat_member(Tchat_id chat_id,
                                                           int_fast32_t user_id);

  inline bool answer_callback_query(std::string const &callback_query_id,
                                    std::string const &text = {},
                                    bool show_alert = false,
                                    std::string const &url = {},
                                    int_fast32_t cache_time = 0);

  // inline mode
  // TODO: answerInlineQuery
};

inline sender::sender(std::string const &this_token,
                      std::string const &this_user_agent) try
  : token(this_token),
    endpoint(LIBTELEGRAM_OUTGOING_PROTO "://api.telegram.org/bot" + this_token + "/"),
    user_agent(this_user_agent) {
  /// Construct a sender with the given token
  urdl_global_options.set_option(urdl::http::max_redirects(0));
  urdl_global_options.set_option(urdl::http::user_agent(user_agent));
  urdl_global_options.set_option(urdl::http::request_method("POST"));
} catch(std::exception const &e) {
  std::cerr << "LibTelegram: Sender: Exception during construction: " << e.what() << std::endl;
}

inline nlohmann::json sender::send_json(std::string const &method,
                                        nlohmann::json const &tree,
                                        unsigned int poll_timeout) {
  /// Function to send a json tree to the given method and get back a response, useful if you want to send custom requests
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: json request:" << std::endl;
    std::cerr << tree.dump(2) << std::endl;
  #endif // NDEBUG

  urdl::istream stream;
  stream.set_options(urdl_global_options);                                      // apply the global options to this stream
  stream.set_option(urdl::http::request_content_type("application/json"));
  stream.set_option(urdl::http::request_content(tree.dump()));                  // write the json dump as the request body
  unsigned int const timeout_ms = poll_timeout * 1000;                          // the stream expects timeouts in milliseconds
  stream.open_timeout(timeout_ms);
  stream.read_timeout(timeout_ms);
  urdl::url const url(endpoint + method);
  stream.open(url);
  if(!stream) {
    std::cerr << "LibTelegram: Sender: Unable to open URL " << url.to_string() << ": " << stream.error().message() << ", headers: " << stream.headers() << std::endl;
    return nlohmann::json();                                                    // return an empty tree
  }

  std::string reply;
  {
    std::string reply_line;
    while(std::getline(stream, reply_line)) {
      reply += reply_line + '\n';                                               // concatenate all lines of input
    }
    reply += reply_line;                                                        // input is not newline-terminated, so don't forget the last line
  }
  if(reply.empty()) {
    std::cerr << "LibTelegram: Sender: Received empty reply to send_json" << std::endl;
    return nlohmann::json();                                                    // return an empty tree
  }
  boost::iostreams::stream<boost::iostreams::array_source> reply_stream(reply.data(), reply.size());
  nlohmann::json reply_tree;                                                    // property tree to contain the reply
  try {
    reply_stream >> reply_tree;
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Sender: Received unparseable JSON in reply to send_json: " << e.what() << std::endl;
  }
  return reply_tree;
}

template<typename T>
inline std::optional<T> sender::send_json_and_parse(std::string const &method,
                                                    nlohmann::json const &tree) {
  /// Wrapper function to send a json object and get back a complete object of the specified template type
  auto reply_tree(send_json(method, tree));
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: json reply:" << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
  #endif // NDEBUG
  if(reply_tree["ok"] != true) {
    std::cerr << "LibTelegram: Sender: Returned status other than OK in reply to " << method << " trying to get " << typeid(T).name() << ":" << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
    return std::nullopt;
  }
  try {
    return types::helpers::make_optional_from_json<T>(reply_tree, "result");
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Sender: Exception parsing the following tree to extract a " << typeid(T).name() << ": " << e.what() << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
    return std::nullopt;
  }
}
template<typename T>
inline std::optional<std::vector<T>> sender::send_json_and_parse_vector(std::string const &method,
                                                                        nlohmann::json const &tree) {
  /// Wrapper function to send a json object and get back a complete object of the specified template type
  auto reply_tree(send_json(method, tree));
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: json vector reply:" << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
  #endif // NDEBUG
  if(reply_tree["ok"] != true) {
    std::cerr << "LibTelegram: Sender: Returned status other than OK in reply to " << method << " trying to get a vector of " << typeid(T).name() << ":" << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
    return std::nullopt;
  }
  try {
    return types::helpers::make_optional_vector_from_json<T>(reply_tree, "result");
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Sender: Exception parsing the following tree to extract a vector of " << typeid(T).name() << ": " << e.what() << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
    return std::nullopt;
  }
}

inline bool sender::send_json_and_get_bool(std::string const &method,
                                           nlohmann::json const &tree) {
  /// Wrapper function to send a json object and get back a boolean result
  auto reply_tree(send_json(method, tree));
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: boolean reply:" << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
  #endif // NDEBUG
  if(reply_tree["ok"] != true) {
    std::cerr << "LibTelegram: Sender: Returned status other than OK in reply to " << method << " expecting a bool:" << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
    return false;
  }
  return reply_tree.at("result");
}

inline std::optional<types::user> const sender::get_me() {
  /// Send a getme request - see https://core.telegram.org/bots/api#getme
  return send_json_and_parse<types::user>("getMe");
}

template<typename Tchat_id>
inline std::optional<types::message> sender::send_message(Tchat_id chat_id,
                                                          std::string const &text,
                                                          int_fast32_t reply_to_message_id,
                                                          parse_mode parse,
                                                          web_preview_mode web_preview,
                                                          notification_mode notification,
                                                          std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                                     types::reply_markup::reply_keyboard_markup,
                                                                                     types::reply_markup::reply_keyboard_remove,
                                                                                     types::reply_markup::force_reply>> reply_markup) {
  /// Send a message to a chat id - see https://core.telegram.org/bots/api#sendmessage
  VERIFY_CHAT_ID
  if(text.empty()) {
    return std::nullopt;                                                        // don't attempt to send empty messages - this would be an error
  }
  if(text.size() > message_length_limit) {                                      // recursively split this message if it's too long
    send_message(chat_id,
                 text.substr(0, message_length_limit),                          // send just the first allowed number of characters in the first half
                 reply_to_message_id,
                 parse,
                 web_preview,
                 notification,
                 reply_markup);
    return send_message(chat_id,
                        text.substr(message_length_limit, std::string::npos),   // send the remaining characters - this will subdivide again recursively if need be
                        reply_to_message_id,
                        parse,
                        web_preview,
                        notification,
                        reply_markup);                                          // note - we disregard return messages from any except the last
  }
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: sending message \"" << text << "\" to chat id " << chat_id << std::endl;
  #endif // NDEBUG
  nlohmann::json tree;                                                          // a json container object for our data
  tree["chat_id"] = chat_id;
  tree["text"] = text;
  if(parse != parse_mode::DEFAULT && parse != parse_mode::UNCHANGED) {          // don't waste bandwidth sending the default option
    switch(parse) {
    case parse_mode::NONE:
      break;
    case parse_mode::MARKDOWN:
      tree["parse_mode"] = "Markdown";
      break;
    case parse_mode::HTML:
      tree["parse_mode"] = "HTML";
      break;
    case parse_mode::UNCHANGED:
      break;
    }
  }
  if(web_preview != web_preview_mode::DEFAULT && web_preview != web_preview_mode::UNCHANGED) { // don't waste bandwidth sending the default option
    switch(web_preview) {
    case web_preview_mode::DISABLE:
      tree["disable_web_page_preview"] = true;
      break;
    case web_preview_mode::ENABLE:
      tree["disable_web_page_preview"] = false;
      break;
    case web_preview_mode::UNCHANGED:
      break;
    }
  }
  if(notification != notification_mode::DEFAULT && notification != notification_mode::UNCHANGED) { // don't waste bandwidth sending the default option
    switch(notification) {
    case notification_mode::DISABLE:
      tree["disable_notification"] = true;
      break;
    case notification_mode::ENABLE:
      tree["disable_notification"] = false;
      break;
    case notification_mode::UNCHANGED:
      break;
    }
  }
  if(reply_to_message_id != reply_to_message_id_none) {
    tree["reply_to_message_id"] = reply_to_message_id;
  }
  if(reply_markup) {
    std::visit([&tree](auto &&arg){arg.get(tree);}, *reply_markup);             // get the tree form of whatever variant we've passed
  }
  return send_json_and_parse<types::message>("sendMessage", tree);
}

template<typename Tchat_id>
inline std::optional<types::message> sender::send_message(Tchat_id chat_id,
                                                          std::string const &text,
                                                          std::variant<types::reply_markup::inline_keyboard_markup,
                                                                       types::reply_markup::reply_keyboard_markup,
                                                                       types::reply_markup::reply_keyboard_remove,
                                                                       types::reply_markup::force_reply> reply_markup,
                                                          int_fast32_t reply_to_message_id,
                                                          parse_mode parse,
                                                          web_preview_mode web_preview,
                                                          notification_mode notification) {
  /// Convenience wrapper to send different argument order to allow others to be used as defaults while providing reply markup first
  VERIFY_CHAT_ID
  return send_message(chat_id, text, reply_to_message_id, parse, web_preview, notification, reply_markup);
}

template<typename Tchat_id>
inline std::optional<types::message> sender::forward_message(Tchat_id chat_id,
                                                             int_fast64_t from_chat_id,
                                                             int_fast32_t message_id,
                                                             notification_mode notification) {
  /// Forward a message to a chat id - see https://core.telegram.org/bots/api#forwardmessage
  VERIFY_CHAT_ID
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: forwarding message " << message_id << " from chat " << from_chat_id << " to chat id " << chat_id << std::endl;
  #endif // NDEBUG
  nlohmann::json tree;                                                          // json object to put our data into
  tree["chat_id"]      = chat_id;
  tree["from_chat_id"] = from_chat_id;
  tree["message_id"]   = message_id;
  if(notification != notification_mode::DEFAULT) {                              // don't waste bandwidth sending the default option
    switch(notification) {
    case notification_mode::DISABLE:
      tree["disable_notification"] = true;
      break;
    case notification_mode::ENABLE:
      tree["disable_notification"] = false;
      break;
    case notification_mode::UNCHANGED:
      break;
    }
  }
  return send_json_and_parse<types::message>("forwardMessage", tree);
}

template<typename Tchat_id>
inline bool sender::edit_message_text(std::string const &text,
                                      Tchat_id chat_id,
                                      int_fast32_t message_id,
                                      parse_mode parse,
                                      web_preview_mode web_preview,
                                      std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                 types::reply_markup::reply_keyboard_markup,
                                                                 types::reply_markup::reply_keyboard_remove,
                                                                 types::reply_markup::force_reply>> reply_markup) {
  /// Edit text and game messages sent by or via the bot - see https://core.telegram.org/bots/api#editmessagetext
  VERIFY_CHAT_ID
  if(text.empty()) {
    return false;                                                               // don't attempt to send empty messages - this would be an error
  }
  nlohmann::json tree;                                                          // a json container object for our data
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: editing message \"" << text << "\" in chat_id " << chat_id << " message_id " << message_id << std::endl;
  #endif // NDEBUG
  tree["chat_id"] = chat_id;
  tree["message_id"] = message_id;
  tree["text"] = text;
  if(parse != parse_mode::UNCHANGED) {                                          // only send values we wish to change
    switch(parse) {
    case parse_mode::NONE:
      tree["parse_mode"] = "";
      break;
    case parse_mode::MARKDOWN:
      tree["parse_mode"] = "Markdown";
      break;
    case parse_mode::HTML:
      tree["parse_mode"] = "HTML";
      break;
    }
  }
  if(web_preview != web_preview_mode::UNCHANGED) {                              // only send values we wish to change
    switch(web_preview) {
    case web_preview_mode::DISABLE:
      tree["disable_web_page_preview"] = true;
      break;
    case web_preview_mode::ENABLE:
      tree["disable_web_page_preview"] = false;
      break;
    }
  }
  if(reply_markup) {
    std::visit([&tree](auto &&arg){arg.get(tree);}, *reply_markup);             // get the tree form of whatever variant we've passed
  }
  return send_json_and_get_bool("editMessageText", tree);
  // TODO: "if edited message is sent by the bot, the edited Message is returned, otherwise True is returned"
}
inline bool sender::edit_message_text(std::string const &text,
                                      std::string const &inline_message_id,
                                      parse_mode parse,
                                      web_preview_mode web_preview,
                                      std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                 types::reply_markup::reply_keyboard_markup,
                                                                 types::reply_markup::reply_keyboard_remove,
                                                                 types::reply_markup::force_reply>> reply_markup) {
  /// Edit text and game messages sent by or via the bot, inline message variant - see https://core.telegram.org/bots/api#editmessagetext
  if(text.empty()) {
    return false;                                                               // don't attempt to send empty messages - this would be an error
  }
  nlohmann::json tree;                                                          // a json container object for our data
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: editing message \"" << text << "\" in inline_message_id " << inline_message_id << std::endl;
  #endif // NDEBUG
  tree["inline_message_id"] = inline_message_id;
  tree["text"] = text;
  switch(parse) {
  case parse_mode::UNCHANGED:                                                   // only send values we wish to change
    break;
  case parse_mode::NONE:
    tree["parse_mode"] = "";
    break;
  case parse_mode::MARKDOWN:
    tree["parse_mode"] = "Markdown";
    break;
  case parse_mode::HTML:
    tree["parse_mode"] = "HTML";
    break;
  }
  switch(web_preview) {
  case web_preview_mode::UNCHANGED:                                             // only send values we wish to change
    break;
  case web_preview_mode::DISABLE:
    tree["disable_web_page_preview"] = true;
    break;
  case web_preview_mode::ENABLE:
    tree["disable_web_page_preview"] = false;
    break;
  }
  if(reply_markup) {
    std::visit([&tree](auto &&arg){arg.get(tree);}, *reply_markup);             // get the tree form of whatever variant we've passed
  }
  return send_json_and_get_bool("editMessageText", tree);
  // TODO: "if edited message is sent by the bot, the edited Message is returned, otherwise True is returned"
}

template<typename Tchat_id = int_fast64_t>
inline bool sender::edit_message_caption(Tchat_id chat_id,
                                         int_fast32_t message_id,
                                         std::string const &caption,
                                         std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                    types::reply_markup::reply_keyboard_markup,
                                                                    types::reply_markup::reply_keyboard_remove,
                                                                    types::reply_markup::force_reply>> reply_markup) {
  /// Edit captions of messages sent by the bot or via the bot (for inline bots). On success, if edited message is sent by the bot, the edited Message is returned, otherwise True is returned - see https://core.telegram.org/bots/api#editmessagecaption
  nlohmann::json tree;                                                          // a json container object for our data
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: editing caption \"" << caption << "\" in chat_id " << chat_id << " message_id " << message_id << std::endl;
  #endif // NDEBUG
  tree["chat_id"] = chat_id;
  tree["message_id"] = message_id;
  tree["caption"] = caption;
  if(reply_markup) {
    std::visit([&tree](auto &&arg){arg.get(tree);}, *reply_markup);             // get the tree form of whatever variant we've passed
  }
  return send_json_and_get_bool("editMessageCaption", tree);
  // TODO: "if edited message is sent by the bot, the edited Message is returned, otherwise True is returned"
}
inline bool sender::edit_message_caption(std::string const &inline_message_id,
                                         std::string const &caption,
                                         std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                    types::reply_markup::reply_keyboard_markup,
                                                                    types::reply_markup::reply_keyboard_remove,
                                                                    types::reply_markup::force_reply>> reply_markup) {
  /// Edit captions of messages sent by the bot or via the bot (for inline bots). On success, if edited message is sent by the bot, the edited Message is returned, otherwise True is returned - see https://core.telegram.org/bots/api#editmessagecaption
  nlohmann::json tree;                                                          // a json container object for our data
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: editing caption \"" << caption << "\" in inline_message_id " << inline_message_id << std::endl;
  #endif // NDEBUG
  tree["inline_message_id"] = inline_message_id;
  tree["caption"] = caption;
  if(reply_markup) {
    std::visit([&tree](auto &&arg){arg.get(tree);}, *reply_markup);             // get the tree form of whatever variant we've passed
  }
  return send_json_and_get_bool("editMessageCaption", tree);
  // TODO: "if edited message is sent by the bot, the edited Message is returned, otherwise True is returned"
}

template<typename Tchat_id = int_fast64_t>
inline bool sender::edit_message_reply_markup(Tchat_id chat_id,
                                              int_fast32_t message_id,
                                              std::variant<types::reply_markup::inline_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_remove,
                                                           types::reply_markup::force_reply> reply_markup) {
  /// Edit only the reply markup of messages sent by the bot or via the bot (for inline bots) - see https://core.telegram.org/bots/api#editmessagereplymarkup
  nlohmann::json tree;                                                          // a json container object for our data
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: editing reply markup in chat_id " << chat_id << " message_id " << message_id << std::endl;
  #endif // NDEBUG
  tree["chat_id"] = chat_id;
  tree["message_id"] = message_id;
  std::visit([&tree](auto &&arg){arg.get(tree);}, reply_markup);                // get the tree form of whatever variant we've passed
  return send_json_and_get_bool("editMessageReplyMarkup", tree);
  // TODO: "if edited message is sent by the bot, the edited Message is returned, otherwise True is returned"
}
inline bool sender::edit_message_reply_markup(std::string const &inline_message_id,
                                              std::variant<types::reply_markup::inline_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_markup,
                                                           types::reply_markup::reply_keyboard_remove,
                                                           types::reply_markup::force_reply> reply_markup) {
  /// Edit only the reply markup of messages sent by the bot or via the bot (for inline bots) - see https://core.telegram.org/bots/api#editmessagereplymarkup
  nlohmann::json tree;                                                          // a json container object for our data
  #ifndef NDEBUG
    std::cerr << "LibTelegram: Sender: DEBUG: editing reply markup in inline_message_id " << inline_message_id << std::endl;
  #endif // NDEBUG
  tree["inline_message_id"] = inline_message_id;
  std::visit([&tree](auto &&arg){arg.get(tree);}, reply_markup);                // get the tree form of whatever variant we've passed
  return send_json_and_get_bool("editMessageReplyMarkup", tree);
  // TODO: "if edited message is sent by the bot, the edited Message is returned, otherwise True is returned"
}


template<typename Tchat_id>
inline std::optional<types::message> sender::send_location(Tchat_id chat_id,
                                                           types::location const &location,
                                                           int_fast32_t live_period,
                                                           bool disable_notification ,
                                                           int_fast32_t reply_to_message_id,
                                                           std::optional<std::variant<types::reply_markup::inline_keyboard_markup,
                                                                                      types::reply_markup::reply_keyboard_markup,
                                                                                      types::reply_markup::reply_keyboard_remove,
                                                                                      types::reply_markup::force_reply>> reply_markup) {
  /// Send a point on a map - see https://core.telegram.org/bots/api#sendlocation
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["latitude"] = location.latitude;
  tree["longitude"] = location.longitude;
  if(live_period != live_location_period_none) {
    tree["live_period"] = live_period;
  }
  if(disable_notification) {
    tree["disable_notification"] = true;
  }
  if(reply_to_message_id != reply_to_message_id_none) {
    tree["reply_to_message_id"] = reply_to_message_id;
  }
  if(reply_markup) {
    std::visit([&tree](auto &&arg){arg.get(tree);}, *reply_markup);             // get the tree form of whatever variant we've passed
  }
  return send_json_and_parse<types::message>("sendLocation", tree);
}

template<typename Tchat_id>
inline std::optional<types::message> sender::edit_message_live_location(Tchat_id chat_id,
                                                                        int_fast32_t message_id,
                                                                        types::location const &location,
                                                                        std::optional<types::reply_markup::inline_keyboard_markup> reply_markup) {
  /// Edit live location messages sent by the bot or via the bot - see https://core.telegram.org/bots/api#editmessagelivelocation
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["message_id"] = message_id;
  tree["latitude"] = location.latitude;
  tree["longitude"] = location.longitude;
  if(reply_markup) {
    reply_markup->get(tree);
  }
  return send_json_and_parse<types::message>("editMessageLiveLocation", tree);
}
inline std::optional<types::message> sender::edit_message_live_location(std::string const &inline_message_id,
                                                                        types::location const &location,
                                                                        std::optional<types::reply_markup::inline_keyboard_markup> reply_markup) {
  /// Edit live location messages sent by the bot or via the bot - see https://core.telegram.org/bots/api#editmessagelivelocation
  nlohmann::json tree;
  tree["inline_message_id"] = inline_message_id;
  tree["latitude"] = location.latitude;
  tree["longitude"] = location.longitude;
  if(reply_markup) {
    reply_markup->get(tree);
  }
  return send_json_and_parse<types::message>("editMessageLiveLocation", tree);
}

template<typename Tchat_id>
inline bool sender::send_chat_action(Tchat_id chat_id,
                                     chat_action_type action) {
  /// Send a chat action - see https://core.telegram.org/bots/api#sendchataction
  /// Return is whether it succeeded
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  switch(action) {
  case chat_action_type::TYPING:
    tree["action"] = "typing";
    break;
  case chat_action_type::UPLOAD_PHOTO:
    tree["action"] = "upload_photo";
    break;
  case chat_action_type::RECORD_VIDEO:
    tree["action"] = "record_video";
    break;
  case chat_action_type::UPLOAD_VIDEO:
    tree["action"] = "upload_video";
    break;
  case chat_action_type::RECORD_AUDIO:
    tree["action"] = "record_audio";
    break;
  case chat_action_type::UPLOAD_AUDIO:
    tree["action"] = "upload_audio";
    break;
  case chat_action_type::UPLOAD_DOCUMENT:
    tree["action"] = "upload_document";
    break;
  case chat_action_type::FIND_LOCATION:
    tree["action"] = "find_location";
    break;
  }
  return send_json_and_get_bool("sendChatAction", tree);
}

inline std::optional<types::file> sender::get_file(std::string const &file_id) {
  /// Get download info about a file (as a file object) - see https://core.telegram.org/bots/api#getfile
  nlohmann::json tree;
  tree["file_id"] = file_id;
  return send_json_and_parse<types::file>("getFile", tree);
}

template<typename Tchat_id>
inline bool sender::kick_chat_member(Tchat_id chat_id,
                                     int_fast32_t user_id,
                                     int_fast32_t until_date) {
  /// Kick a user and ban them from the chat - see https://core.telegram.org/bots/api#kickchatmember
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  if(until_date != 0) {
    tree["until_date"] = until_date;
  }
  return send_json_and_get_bool("kickChatMember", tree);
}

template<typename Tchat_id>
inline bool sender::unban_chat_member(Tchat_id chat_id,
                                      int_fast32_t user_id) {
  /// Unban a user from the chat - see https://core.telegram.org/bots/api#unbanchatmember
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  return send_json_and_get_bool("unbanChatMember", tree);
}

template<typename Tchat_id>
inline bool sender::restrict_chat_member(Tchat_id chat_id,
                                         int_fast32_t user_id,
                                         int_fast32_t until_date,
                                         bool can_send_messages,
                                         bool can_send_media_messages,
                                         bool can_send_other_messages,
                                         bool can_add_web_page_previews) {
  /// Restrict a user in a supergroup, numerical variant. The bot must be an administrator in the supergroup for this to work and must have the appropriate admin rights. Pass True for all boolean parameters to lift restrictions from a user. - see https://core.telegram.org/bots/api#restrictchatmember
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  if(until_date) {
    tree["until_date"] = until_date;
  }
  if(can_send_messages) {
    tree["can_send_messages"] = can_send_messages;
  }
  if(can_send_media_messages) {
    tree["can_send_media_messages"] = can_send_media_messages;
  }
  if(can_send_other_messages) {
    tree["can_send_other_messages"] = can_send_other_messages;
  }
  if(can_add_web_page_previews) {
    tree["can_add_web_page_previews"] = can_add_web_page_previews;
  }
  return send_json_and_get_bool("restrictChatMember", tree);
}

template<typename Tchat_id>
inline bool sender::promote_chat_member(Tchat_id chat_id,
                                        int_fast32_t user_id,
                                        bool can_change_info,
                                        bool can_post_messages,
                                        bool can_edit_messages,
                                        bool can_delete_messages,
                                        bool can_invite_users,
                                        bool can_restrict_members,
                                        bool can_pin_messages,
                                        bool can_promote_members) {
  /// Promote or demote a user in a supergroup or a channel. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Pass False for all boolean parameters to demote a user. - see https://core.telegram.org/bots/api#promotechatmember
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  if(can_change_info) {
    tree["can_change_info"] = can_change_info;
  }
  if(can_post_messages) {
    tree["can_post_messages"] = can_post_messages;
  }
  if(can_edit_messages) {
    tree["can_edit_messages"] = can_edit_messages;
  }
  if(can_delete_messages) {
    tree["can_delete_messages"] = can_delete_messages;
  }
  if(can_invite_users) {
    tree["can_invite_users"] = can_invite_users;
  }
  if(can_restrict_members) {
    tree["can_restrict_members"] = can_restrict_members;
  }
  if(can_pin_messages) {
    tree["can_pin_messages"] = can_pin_messages;
  }
  if(can_promote_members) {
    tree["can_promote_members"] = can_promote_members;
  }
  return send_json_and_get_bool("promoteChatMember", tree);
}

// TODO: exportChatInviteLink
// TODO: setChatPhoto

template<typename Tchat_id>
inline bool sender::delete_chat_photo(Tchat_id chat_id) {
  /// Delete a chat photo. Photos can't be changed for private chats. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights.
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_get_bool("deleteChatPhoto", tree);
}

template<typename Tchat_id>
inline bool sender::set_chat_title(Tchat_id chat_id,
                                   std::string const &title) {
  /// Change the title of a chat. Titles can't be changed for private chats. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. - see https://core.telegram.org/bots/api#setchattitle
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  if(!title.empty()) {
    tree["title"] = title;
  }
  return send_json_and_get_bool("setChatTitle", tree);
}

template<typename Tchat_id>
inline bool sender::set_chat_description(Tchat_id chat_id,
                                         std::string const &description) {
  /// Change the description of a supergroup or a channel. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. - see https://core.telegram.org/bots/api#setchatdescription
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  if(!description.empty()) {
    tree["description"] = description;
  }
  return send_json_and_get_bool("setChatDescription", tree);
}

template<typename Tchat_id>
inline bool sender::pin_chat_message(Tchat_id chat_id,
                                     int_fast32_t message_id,
                                     bool disable_notification) {
  /// Unpin a message in a supergroup chat. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. - see https://core.telegram.org/bots/api#pinchatmessage
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["message_id"] = message_id;
  if(disable_notification) {
    tree["disable_notification"] = true;
  }
  return send_json_and_get_bool("pinChatMessage", tree);
}

template<typename Tchat_id>
inline bool sender::unpin_chat_message(Tchat_id chat_id) {
  /// Unpin a message in a supergroup chat. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. - see https://core.telegram.org/bots/api#unpinchatmessage
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_get_bool("unpinChatMessage", tree);
}

template<typename Tchat_id>
inline bool sender::leave_chat(Tchat_id chat_id) {
  /// Leave a group or a channel - see https://core.telegram.org/bots/api#leavechat
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_get_bool("leaveChat", tree);
}

template<typename Tchat_id>
inline std::optional<types::chat> sender::get_chat(Tchat_id chat_id) {
  /// Get latest information about a chat id- see https://core.telegram.org/bots/api#getchat
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse<types::chat>("getChat", tree);
}

template<typename Tchat_id>
inline std::optional<std::vector<types::chat_member>> sender::get_chat_administrators(Tchat_id chat_id) {
  /// Get a list of administrators in a chat- see https://core.telegram.org/bots/api#getchatadministrators
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse_vector<types::chat_member>("getChatAdministrators", tree);
}

template<typename Tchat_id>
inline std::optional<int_fast32_t> sender::get_chat_members_count(Tchat_id chat_id) {
  /// Get a count of users in a chat - see https://core.telegram.org/bots/api#getchatmemberscount
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse<int_fast32_t>("getChatMembersCount", tree);
}

template<typename Tchat_id>
inline std::optional<types::chat_member> sender::get_chat_member(Tchat_id chat_id,
                                                                 int_fast32_t user_id) {
  /// Get a chat member by id - see https://core.telegram.org/bots/api#getchatmember
  VERIFY_CHAT_ID
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  return send_json_and_parse<types::chat_member>("getChatMember", tree);
}

inline bool sender::answer_callback_query(std::string const &callback_query_id,
                                          std::string const &text,
                                          bool show_alert,
                                          std::string const &url,
                                          int_fast32_t cache_time) {
  /// Send an acknowledgement to a callback query - see https://core.telegram.org/bots/api#answercallbackquery
  nlohmann::json tree;
  tree["callback_query_id"] = callback_query_id;
  if(!text.empty()) {
    tree["text"] = text;
  }
  if(show_alert) {
    tree["show_alert"] = true;
  }
  if(!url.empty()) {
    tree["url"] = url;
  }
  if(cache_time != 0) {
    tree["cache_time"] = cache_time;
  }
  return send_json_and_get_bool("answerCallbackQuery", tree);
}

}

#endif // TELEGRAM_SENDER_H_INCLUDED
