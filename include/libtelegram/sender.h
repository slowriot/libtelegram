#ifndef TELEGRAM_SENDER_H_INCLUDED
#define TELEGRAM_SENDER_H_INCLUDED

#include <string>
#include <urdl/istream.hpp>

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
    DEFAULT = NONE
  };
  enum class web_preview_mode : bool {                                          // whether or not to allow the web preview for links, see https://core.telegram.org/bots/api#sendmessage
    DISABLE,
    ENABLE,
    DEFAULT = ENABLE
  };
  enum class notification_mode : bool {                                         // whether to send the message silently, see https://core.telegram.org/bots/api#sendmessage
    DISABLE,
    ENABLE,
    DEFAULT = ENABLE
  };

  // TODO: handle sendMessage.reply_markup

  sender(std::string const &token, std::string const &user_agent = "LibTelegram");

  void send_message(int_fast64_t chat_id,
                    std::string const &text,
                    parse_mode parse_mode = parse_mode::DEFAULT,
                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                    notification_mode notification = notification_mode::DEFAULT,
                    int_fast32_t reply_to_message_id = -1);
  void send_message(std::string channel_name,
                    std::string const &text,
                    parse_mode parse_mode = parse_mode::DEFAULT,
                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                    notification_mode notification = notification_mode::DEFAULT,
                    int_fast32_t reply_to_message_id = -1);
};

sender::sender(std::string const &this_token,
               std::string const &this_user_agent)
  : token(this_token),
    endpoint("https://api.telegram.org/bot" + token + "/"),
    user_agent(user_agent) {
  /// Construct a sender with the given token
  urdl_global_options.set_option(urdl::http::max_redirects(0));
  urdl_global_options.set_option(urdl::http::user_agent(user_agent));
  urdl_global_options.set_option(urdl::http::request_method("POST"));
}

void sender::send_message(int_fast64_t chat_id,
                          std::string const &text,
                          parse_mode parse_mode,
                          web_preview_mode web_preview,
                          notification_mode notification,
                          int_fast32_t reply_to_message_id) {
  /// Send a message to a chat id
  std::cerr << "DEBUG: sending message \"" << text << "\" to chat id " << chat_id << std::endl;
  // application/json
  urdl::istream stream;
  stream.set_options(urdl_global_options);
  stream.set_option(urdl::http::request_content_type("application/json"));
  stream.set_option(urdl::http::request_content("Hello, world!"));
  stream.open_timeout(60000);
  stream.read_timeout(30000);
  stream.open(endpoint + "sendMessage");
  if(!stream) {
    std::cerr << "LibTelegram: Sender: Unable to open URL: " << stream.error().message() << std::endl;
    return;
  }

  boost::property_tree::ptree reply_tree;                                       // property tree to contain the reply
  try {
    boost::property_tree::read_json(stream, reply_tree);                        // parse the reply json into the property tree
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Received unparseable JSON in reply to send_message: " << e.what() << std::endl;
  }
  // TODO: check the status of the reply
}
void sender::send_message(std::string channel_name,
                          std::string const &text,
                          parse_mode parse_mode,
                          web_preview_mode web_preview,
                          notification_mode notification,
                          int_fast32_t reply_to_message_id) {
  /// Send a message to a channel name

}

}

#endif // TELEGRAM_SENDER_H_INCLUDED
