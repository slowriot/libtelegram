#ifndef TELEGRAM_SENDER_H_INCLUDED
#define TELEGRAM_SENDER_H_INCLUDED

#include <string>
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
#include "types/user.h"

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
    UPLOAD_VIDEO,                                                               // pload_video for videos
    RECORD_AUDIO,                                                               // record_audio for audio files
    UPLOAD_AUDIO,                                                               // upload_audio for audio files
    UPLOAD_DOCUMENT,                                                            // upload_document for general files
    FIND_LOCATION                                                               // find_location for location data.
  };
  static int_fast32_t constexpr const reply_to_message_id_none = -1;
  static int_fast32_t constexpr const message_length_limit = 4096;              // see https://core.telegram.org/method/messages.sendMessage

  // TODO: add message sending stream class
  // TODO: add statistics on bytes sent and received

  inline sender(std::string const &token, std::string const &user_agent = "LibTelegram");

  inline nlohmann::json send_json(std::string const &method,
                                  nlohmann::json const &tree = {},
                                  unsigned int poll_timeout = 30);
  template<typename T>
  inline std::experimental::optional<T> send_json_and_parse(std::string const &method,
                                                            nlohmann::json const &tree = {});
  template<typename T>
  inline std::experimental::optional<std::vector<T>> send_json_and_parse_vector(std::string const &method,
                                                                                nlohmann::json const &tree = {});
  inline bool send_json_and_get_bool(std::string const &method,
                                     nlohmann::json const &tree = {});

  inline std::experimental::optional<types::user> const get_me();

  template<typename Treply_markup = types::reply_markup::force_reply>
  inline std::experimental::optional<types::message> send_message(int_fast64_t chat_id,
                                                                  std::string const &text,
                                                                  int_fast32_t reply_to_message_id = reply_to_message_id_none,
                                                                  parse_mode parse = parse_mode::DEFAULT,
                                                                  web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                                                  notification_mode notification = notification_mode::DEFAULT,
                                                                  types::reply_markup::base<Treply_markup> const *reply_markup = nullptr);
  template<typename Treply_markup = types::reply_markup::force_reply>
  inline std::experimental::optional<types::message> send_message(std::string const &chat_id,
                                                                  std::string const &text,
                                                                  int_fast32_t reply_to_message_id = reply_to_message_id_none,
                                                                  parse_mode parse = parse_mode::DEFAULT,
                                                                  web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                                                  notification_mode notification = notification_mode::DEFAULT,
                                                                  types::reply_markup::base<Treply_markup> const *reply_markup = nullptr);

  inline std::experimental::optional<types::message> forward_message(int_fast64_t chat_id,
                                                                     int_fast64_t from_chat_id,
                                                                     int_fast32_t message_id,
                                                                     notification_mode notification = notification_mode::DEFAULT);
  inline std::experimental::optional<types::message> forward_message(std::string const &chat_id,
                                                                     int_fast64_t from_chat_id,
                                                                     int_fast32_t message_id,
                                                                     notification_mode notification = notification_mode::DEFAULT);

  // updating messages
  template<typename Treply_markup = types::reply_markup::force_reply>
  inline bool edit_message_text(std::string const &text,
                                int_fast64_t chat_id,
                                int_fast32_t message_id,
                                parse_mode parse = parse_mode::DEFAULT,
                                web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                types::reply_markup::base<Treply_markup> const *reply_markup = nullptr);
  template<typename Treply_markup = types::reply_markup::force_reply>
  inline bool edit_message_text(std::string const &text,
                                std::string const &chat_id,
                                int_fast32_t message_id,
                                parse_mode parse = parse_mode::DEFAULT,
                                web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                types::reply_markup::base<Treply_markup> const *reply_markup = nullptr);
  template<typename Treply_markup = types::reply_markup::force_reply>
  inline bool edit_message_text(std::string const &text,
                                std::string const &inline_message_id = {},
                                parse_mode parse = parse_mode::DEFAULT,
                                web_preview_mode web_preview = web_preview_mode::DEFAULT,
                                types::reply_markup::base<Treply_markup> const *reply_markup = nullptr);
  // TODO: editMessageText
  // TODO: editMessageCaption
  // TODO: editMessageReplyMarkup
  // TODO: deleteMessage

  // TODO: sendPhoto
  // TODO: sendAudio
  // TODO: sendDocument
  // TODO: sendSticker
  // TODO: sendVideo
  // TODO: sendVoice
  // TODO: sendVideoNote
  // TODO: sendLocation
  // TODO: sendVenue
  // TODO: sendContact

  inline bool send_chat_action(int_fast64_t chat_id,
                               chat_action_type action = chat_action_type::TYPING);
  inline bool send_chat_action(std::string const &chat_id,
                               chat_action_type action = chat_action_type::TYPING);

  // TODO: getUserProfilePhotos

  inline std::experimental::optional<types::file> get_file(std::string const &file_id);

  inline bool kick_chat_member(int_fast64_t chat_id,
                               int_fast32_t user_id);
  inline bool kick_chat_member(std::string const &chat_id,
                               int_fast32_t user_id);

  inline bool unban_chat_member(int_fast64_t chat_id,
                                int_fast32_t user_id);
  inline bool unban_chat_member(std::string const &chat_id,
                                int_fast32_t user_id);

  inline bool leave_chat(int_fast64_t chat_id);
  inline bool leave_chat(std::string const &chat_id);

  inline std::experimental::optional<types::chat> get_chat(int_fast64_t chat_id);
  inline std::experimental::optional<types::chat> get_chat(std::string const &chat_id);

  std::experimental::optional<std::vector<types::chat_member>> get_chat_administrators(int_fast64_t chat_id);
  std::experimental::optional<std::vector<types::chat_member>> get_chat_administrators(std::string const &chat_id);

  inline std::experimental::optional<int_fast32_t> get_chat_members_count(int_fast64_t chat_id);
  inline std::experimental::optional<int_fast32_t> get_chat_members_count(std::string const &chat_id);

  inline std::experimental::optional<types::chat_member> get_chat_member(int_fast64_t chat_id,
                                                                         int_fast32_t user_id);
  inline std::experimental::optional<types::chat_member> get_chat_member(std::string const &chat_id,
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
inline std::experimental::optional<T> sender::send_json_and_parse(std::string const &method,
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
    return std::experimental::nullopt;
  }
  try {
    return types::make_optional<T>(reply_tree, "result");
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Sender: Exception parsing the following tree to extract a " << typeid(T).name() << ": " << e.what() << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
    return std::experimental::nullopt;
  }
}
template<typename T>
inline std::experimental::optional<std::vector<T>> sender::send_json_and_parse_vector(std::string const &method,
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
    return std::experimental::nullopt;
  }
  try {
    return types::make_optional_vector<T>(reply_tree, "result");
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Sender: Exception parsing the following tree to extract a vector of " << typeid(T).name() << ": " << e.what() << std::endl;
    std::cerr << reply_tree.dump(2) << std::endl;
    return std::experimental::nullopt;
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

inline std::experimental::optional<types::user> const sender::get_me() {
  /// Send a getme request - see https://core.telegram.org/bots/api#getme
  return send_json_and_parse<types::user>("getMe");
}

template<typename Treply_markup>
inline std::experimental::optional<types::message> sender::send_message(int_fast64_t chat_id,
                                                                        std::string const &text,
                                                                        int_fast32_t reply_to_message_id,
                                                                        parse_mode parse,
                                                                        web_preview_mode web_preview,
                                                                        notification_mode notification,
                                                                        types::reply_markup::base<Treply_markup> const *reply_markup) {
  /// Send a message to a chat id - see https://core.telegram.org/bots/api#sendmessage
  if(text.empty()) {
    return std::experimental::nullopt;                                          // don't attempt to send empty messages - this would be an error
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
    }
  }
  if(reply_to_message_id != reply_to_message_id_none) {
    tree["reply_to_message_id"] = reply_to_message_id;
  }
  if(reply_markup) {
    reply_markup->get(tree);
  }
  return send_json_and_parse<types::message>("sendMessage", tree);
}
template<typename Treply_markup>
inline std::experimental::optional<types::message> sender::send_message(std::string const &chat_id,
                                                                        std::string const &text,
                                                                        int_fast32_t reply_to_message_id,
                                                                        parse_mode parse,
                                                                        web_preview_mode web_preview,
                                                                        notification_mode notification,
                                                                        types::reply_markup::base<Treply_markup> const *reply_markup) {
  /// Send a message to a channel name - see https://core.telegram.org/bots/api#sendmessage
  if(text.empty()) {
    return std::experimental::nullopt;                                          // don't attempt to send empty messages - this would be an error
  }
  if(text.size() > message_length_limit) {                                      // recursively split this message if it's too long
    send_message(chat_id,
                 text.substr(0, message_length_limit),                          // send just the first allowed number of characters in the first half
                 reply_to_message_id,
                 parse,
                 web_preview,
                 notification);
    return send_message(chat_id,
                        text.substr(message_length_limit, std::string::npos),   // send the remaining characters - this will subdivide again recursively if need be
                        reply_to_message_id,
                        parse,
                        web_preview,
                        notification);                                          // note - we disregard return messages from any except the last
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
    }
  }
  if(reply_to_message_id != reply_to_message_id_none) {
    tree["reply_to_message_id"] = reply_to_message_id;
  }
  if(reply_markup) {
    reply_markup->get(tree);
  }
  return send_json_and_parse<types::message>("sendMessage", tree);
}

inline std::experimental::optional<types::message> sender::forward_message(int_fast64_t chat_id,
                                                                           int_fast64_t from_chat_id,
                                                                           int_fast32_t message_id,
                                                                           notification_mode notification) {
  /// Forward a message to a chat id - see https://core.telegram.org/bots/api#forwardmessage
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
    }
  }
  return send_json_and_parse<types::message>("forwardMessage", tree);
}
inline std::experimental::optional<types::message> sender::forward_message(std::string const &chat_id,
                                                                           int_fast64_t from_chat_id,
                                                                           int_fast32_t message_id,
                                                                           notification_mode notification) {
  /// Forward a message to a chat id - see https://core.telegram.org/bots/api#forwardmessage
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
    }
  }
  return send_json_and_parse<types::message>("forwardMessage", tree);
}

template<typename Treply_markup>
inline bool sender::edit_message_text(std::string const &text,
                                      int_fast64_t chat_id,
                                      int_fast32_t message_id,
                                      parse_mode parse,
                                      web_preview_mode web_preview,
                                      types::reply_markup::base<Treply_markup> const *reply_markup) {
  /// Edit text and game messages sent by or via the bot, numerical chat id variant - see https://core.telegram.org/bots/api#editmessagetext
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
    reply_markup->get(tree);
  }
  return send_json_and_get_bool("editMessageText", tree);
}
template<typename Treply_markup>
inline bool sender::edit_message_text(std::string const &text,
                                      std::string const &chat_id,
                                      int_fast32_t message_id,
                                      parse_mode parse,
                                      web_preview_mode web_preview,
                                      types::reply_markup::base<Treply_markup> const *reply_markup) {
  /// Edit text and game messages sent by or via the bot, string chat id variant - see https://core.telegram.org/bots/api#editmessagetext
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
    reply_markup->get(tree);
  }
  return send_json_and_get_bool("editMessageText", tree);
}
template<typename Treply_markup>
inline bool sender::edit_message_text(std::string const &text,
                                      std::string const &inline_message_id,
                                      parse_mode parse,
                                      web_preview_mode web_preview,
                                      types::reply_markup::base<Treply_markup> const *reply_markup) {
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
    reply_markup->get(tree);
  }
  return send_json_and_get_bool("editMessageText", tree);
}

inline bool sender::send_chat_action(int_fast64_t chat_id,
                                     chat_action_type action) {
  /// Send a chat action - see https://core.telegram.org/bots/api#sendchataction
  /// Return is whether it succeeded
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
inline bool sender::send_chat_action(std::string const &chat_id,
                                     chat_action_type action) {
  /// Send a chat action - see https://core.telegram.org/bots/api#sendchataction
  /// Return is whether it succeeded
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

inline std::experimental::optional<types::file> sender::get_file(std::string const &file_id) {
  /// Get download info about a file (as a file object) - see https://core.telegram.org/bots/api#getfile
  nlohmann::json tree;
  tree["file_id"] = file_id;
  return send_json_and_parse<types::file>("getFile", tree);
}

inline bool sender::kick_chat_member(int_fast64_t chat_id,
                                     int_fast32_t user_id) {
  /// Kick a user and ban them from the chat, numerical chat id variant - see https://core.telegram.org/bots/api#kickchatmember
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  return send_json_and_get_bool("kickChatMember", tree);
}
inline bool sender::kick_chat_member(std::string const &chat_id,
                                     int_fast32_t user_id) {
  /// Kick a user and ban them from the chat, string supergroup name variant - see https://core.telegram.org/bots/api#kickchatmember
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  return send_json_and_get_bool("kickChatMember", tree);
}

inline bool sender::unban_chat_member(int_fast64_t chat_id,
                                      int_fast32_t user_id) {
  /// Unban a user from the chat, numerical chat id variant - see https://core.telegram.org/bots/api#unbanchatmember
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  return send_json_and_get_bool("unbanChatMember", tree);
}
inline bool sender::unban_chat_member(std::string const &chat_id,
                                      int_fast32_t user_id) {
  /// Unban a user from the chat, string supergroup name variant - see https://core.telegram.org/bots/api#unbanchatmember
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  return send_json_and_get_bool("unbanChatMember", tree);
}

inline bool sender::leave_chat(int_fast64_t chat_id) {
  /// Leave a group or a channel, numerical chat id variant - see https://core.telegram.org/bots/api#leavechat
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_get_bool("leaveChat", tree);
}
inline bool sender::leave_chat(std::string const &chat_id) {
  /// Leave a group or a channel, string supergroup name variant - see https://core.telegram.org/bots/api#leavechat
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_get_bool("leaveChat", tree);
}

inline std::experimental::optional<types::chat> sender::get_chat(int_fast64_t chat_id) {
  /// Get latest information about a chat id, numerical chat id variant- see https://core.telegram.org/bots/api#getchat
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse<types::chat>("getChat", tree);
}
inline std::experimental::optional<types::chat> sender::get_chat(std::string const &chat_id) {
  /// Get latest information about a chat id, string supergroup name variant - see https://core.telegram.org/bots/api#getchat
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse<types::chat>("getChat", tree);
}

std::experimental::optional<std::vector<types::chat_member>> sender::get_chat_administrators(int_fast64_t chat_id) {
  /// Get a list of administrators in a chat, numerical chat id variant- see https://core.telegram.org/bots/api#getchatadministrators
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse_vector<types::chat_member>("getChatAdministrators", tree);
}
std::experimental::optional<std::vector<types::chat_member>> sender::get_chat_administrators(std::string const &chat_id) {
  /// Get a list of administrators in a chat, string supergroup name variant - see https://core.telegram.org/bots/api#getchatadministrators
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse_vector<types::chat_member>("getChatAdministrators", tree);
}

inline std::experimental::optional<int_fast32_t> sender::get_chat_members_count(int_fast64_t chat_id) {
  /// Get a count of users in a chat, numerical chat id variant - see https://core.telegram.org/bots/api#getchatmemberscount
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse<int_fast32_t>("getChatMembersCount", tree);
}
inline std::experimental::optional<int_fast32_t> sender::get_chat_members_count(std::string const &chat_id) {
  /// Get a count of users in a chat, string supergroup name variant - see https://core.telegram.org/bots/api#getchatmemberscount
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  return send_json_and_parse<int_fast32_t>("getChatMembersCount", tree);
}

inline std::experimental::optional<types::chat_member> sender::get_chat_member(int_fast64_t chat_id,
                                                                               int_fast32_t user_id) {
  /// Get a chat member by id, numerical chat id variant - see https://core.telegram.org/bots/api#getchatmember
  nlohmann::json tree;
  tree["chat_id"] = chat_id;
  tree["user_id"] = user_id;
  return send_json_and_parse<types::chat_member>("getChatMember", tree);
}
inline std::experimental::optional<types::chat_member> sender::get_chat_member(std::string const &chat_id,
                                                                               int_fast32_t user_id) {
  /// Get a chat member by id, string supergroup name variant - see https://core.telegram.org/bots/api#getchatmember
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
