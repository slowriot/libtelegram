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
  static int_fast32_t constexpr const reply_to_message_id_none = -1;

  sender(std::string const &token, std::string const &user_agent = "LibTelegram");

  void send_message(int_fast64_t chat_id,
                    std::string const &text,
                    parse_mode parse = parse_mode::DEFAULT,
                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                    notification_mode notification = notification_mode::DEFAULT,
                    int_fast32_t reply_to_message_id = reply_to_message_id_none);
  void send_message(std::string channel_name,
                    std::string const &text,
                    parse_mode parse = parse_mode::DEFAULT,
                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                    notification_mode notification = notification_mode::DEFAULT,
                    int_fast32_t reply_to_message_id = reply_to_message_id_none);
};

sender::sender(std::string const &this_token,
               std::string const &this_user_agent)
  : token(this_token),
    endpoint("https://api.telegram.org/bot" + this_token + "/"),
    user_agent(user_agent) {
  /// Construct a sender with the given token
  urdl_global_options.set_option(urdl::http::max_redirects(0));
  urdl_global_options.set_option(urdl::http::user_agent(user_agent));
  urdl_global_options.set_option(urdl::http::request_method("POST"));
}

void sender::send_message(int_fast64_t chat_id,
                          std::string const &text,
                          parse_mode parse,
                          web_preview_mode web_preview,
                          notification_mode notification,
                          int_fast32_t reply_to_message_id) {
  /// Send a message to a chat id
  std::cerr << "DEBUG: sending message \"" << text << "\" to chat id " << chat_id << std::endl;
  // application/json
  boost::property_tree::ptree tree;                                             // a property tree to put our data into
  tree.put("chat_id", chat_id);
  tree.put("text",    text);
  if(parse != parse_mode::DEFAULT) {                                            // don't waste bandwidth sending the default option
    switch(parse) {
    case parse_mode::NONE:
      break;
    case parse_mode::MARKDOWN:
      tree.put("parse_mode", "Markdown");
      break;
    case parse_mode::HTML:
      tree.put("parse_mode", "HTML");
      break;
    }
  }
  if(web_preview != web_preview_mode::DEFAULT) {                                // don't waste bandwidth sending the default option
    switch(web_preview) {
    case web_preview_mode::DISABLE:
      tree.put("disable_web_page_preview", true);
      break;
    case web_preview_mode::ENABLE:
      tree.put("disable_web_page_preview", false);
      break;
    }
  }
  if(notification != notification_mode::DEFAULT) {                              // don't waste bandwidth sending the default option
    switch(notification) {
    case notification_mode::DISABLE:
      tree.put("disable_notification", true);
      break;
    case notification_mode::ENABLE:
      tree.put("disable_notification", false);
      break;
    }
  }
  if(reply_to_message_id != reply_to_message_id_none) {
    tree.put("reply_to_message_id", reply_to_message_id);
  }
  // TODO: handle sendMessage.reply_markup
  std::ostringstream ss;                                                        // a stream buffer for the encoded json
  std::cerr << "LibTelegram: Sender: DEBUG: json request:" << std::endl;
  boost::property_tree::write_json(std::cerr, tree);
  boost::property_tree::write_json(ss, tree);                                   // encode our data as json

  urdl::istream stream;
  stream.set_options(urdl_global_options);                                      // apply the global options to this stream
  stream.set_option(urdl::http::request_content_type("application/json"));
  ///stream.set_option(urdl::http::request_content(ss.str()));                     // write the stringstream as the request body
  stream.set_option(urdl::http::request_content("getMe"));                      // test
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
  std::cerr << "LibTelegram: Sender: DEBUG: json reply:" << std::endl;
  boost::property_tree::write_json(std::cerr, reply_tree);
}
void sender::send_message(std::string channel_name,
                          std::string const &text,
                          parse_mode parse,
                          web_preview_mode web_preview,
                          notification_mode notification,
                          int_fast32_t reply_to_message_id) {
  /// Send a message to a channel name

}

}

#endif // TELEGRAM_SENDER_H_INCLUDED
