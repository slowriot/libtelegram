#ifndef TELEGRAM_SENDER_H_INCLUDED
#define TELEGRAM_SENDER_H_INCLUDED

#include <string>
#define URDL_HEADER_ONLY 1
#ifdef LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING
  #warning "SSL is disabled for outgoing messages - that is such a bad idea."
  #define URDL_DISABLE_SSL=1
  #define LIBTELEGRAM_OUTGOING_PROTO "http"
#else
  #define LIBTELEGRAM_OUTGOING_PROTO "https"
#endif // LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING
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
  enum class web_preview_mode : char {                                          // whether or not to allow the web preview for links, see https://core.telegram.org/bots/api#sendmessage
    DISABLE,
    ENABLE,
    DEFAULT = ENABLE
  };
  enum class notification_mode : char {                                         // whether to send the message silently, see https://core.telegram.org/bots/api#sendmessage
    DISABLE,
    ENABLE,
    DEFAULT = ENABLE
  };
  static int_fast32_t constexpr const reply_to_message_id_none = -1;
  static int_fast32_t constexpr const message_length_limit = 4096;              // see https://core.telegram.org/method/messages.sendMessage

  sender(std::string const &token, std::string const &user_agent = "LibTelegram");

  boost::property_tree::ptree send_json(std::string const &method, boost::property_tree::ptree const &tree);

  void send_message(int_fast64_t chat_id,
                    std::string const &text,
                    int_fast32_t reply_to_message_id = reply_to_message_id_none,
                    parse_mode parse = parse_mode::DEFAULT,
                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                    notification_mode notification = notification_mode::DEFAULT);
  void send_message(std::string channel_name,
                    std::string const &text,
                    int_fast32_t reply_to_message_id = reply_to_message_id_none,
                    parse_mode parse = parse_mode::DEFAULT,
                    web_preview_mode web_preview = web_preview_mode::DEFAULT,
                    notification_mode notification = notification_mode::DEFAULT);
};

sender::sender(std::string const &this_token,
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

boost::property_tree::ptree sender::send_json(std::string const &method,
                                              boost::property_tree::ptree const &tree) {
  /// Function to json-encode and send a property tree to the given method, useful if you want to send custom requests
  std::ostringstream ss;                                                        // a stream buffer for the encoded json
  std::cerr << "LibTelegram: Sender: DEBUG: json request:" << std::endl;
  boost::property_tree::write_json(std::cerr, tree);
  boost::property_tree::write_json(ss, tree);                                   // encode our data as json

  urdl::istream stream;
  stream.set_options(urdl_global_options);                                      // apply the global options to this stream
  stream.set_option(urdl::http::request_content_type("application/json"));
  stream.set_option(urdl::http::request_content(ss.str()));                     // write the stringstream as the request body
  stream.open_timeout(60000);
  stream.read_timeout(30000);
  urdl::url const url(endpoint + method);
  stream.open(url);
  if(!stream) {
    std::cerr << "LibTelegram: Sender: Unable to open URL " << url.to_string() << ": " << stream.error().message() << std::endl;
    return boost::property_tree::ptree();                                       // return an empty tree
  }

  std::string reply;
  std::getline(stream, reply);                                                  // this returns false as it finishes without a newline
  if(reply.empty()) {
    std::cerr << "LibTelegram: Sender: Received empty reply to send_json" << std::endl;
    return boost::property_tree::ptree();                                       // return an empty tree
  }
  boost::iostreams::stream<boost::iostreams::array_source> reply_stream(reply.data(), reply.size());
  boost::property_tree::ptree reply_tree;                                       // property tree to contain the reply
  try {
    boost::property_tree::read_json(reply_stream, reply_tree);                  // parse the reply json into the property tree
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Sender: Received unparseable JSON in reply to send_json: " << e.what() << std::endl;
  }
  return reply_tree;
}

void sender::send_message(int_fast64_t chat_id,
                          std::string const &text,
                          int_fast32_t reply_to_message_id,
                          parse_mode parse,
                          web_preview_mode web_preview,
                          notification_mode notification) {
  /// Send a message to a chat id
  if(text.empty()) {
    return;                                                                     // don't attempt to send empty messages
  }
  if(text.size() > message_length_limit) {                                      // recursively split this message if it's too long
    send_message(chat_id,
                 text.substr(0, message_length_limit),                          // send just the first allowed number of characters in the first half
                 reply_to_message_id,
                 parse,
                 web_preview,
                 notification);
    send_message(chat_id,
                 text.substr(message_length_limit, std::string::npos),          // send the remaining characters - this will subdivide again recursively if need be
                 reply_to_message_id,
                 parse,
                 web_preview,
                 notification);
    return;
  }
  std::cerr << "DEBUG: sending message \"" << text << "\" to chat id " << chat_id << std::endl;
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
  boost::property_tree::ptree reply_tree(send_json("sendMessage", tree));
  if(reply_tree.get("ok", "") != "true") {
    std::cerr << "LibTelegram: Sender: Returned status other than OK in reply to send_message:" << std::endl;
    boost::property_tree::write_json(std::cerr, reply_tree);
  }
}
void sender::send_message(std::string channel_name,
                          std::string const &text,
                          int_fast32_t reply_to_message_id,
                          parse_mode parse,
                          web_preview_mode web_preview,
                          notification_mode notification) {
  /// Send a message to a channel name
  if(text.empty()) {
    return;                                                                     // don't attempt to send empty messages
  }
  if(text.size() > message_length_limit) {                                      // recursively split this message if it's too long
    send_message(channel_name,
                 text.substr(0, message_length_limit),                          // send just the first allowed number of characters in the first half
                 reply_to_message_id,
                 parse,
                 web_preview,
                 notification);
    send_message(channel_name,
                 text.substr(message_length_limit, std::string::npos),          // send the remaining characters - this will subdivide again recursively if need be
                 reply_to_message_id,
                 parse,
                 web_preview,
                 notification);
    return;
  }
  std::cerr << "DEBUG: sending message \"" << text << "\" to channel name " << channel_name << std::endl;
  boost::property_tree::ptree tree;                                             // a property tree to put our data into
  tree.put("chat_id", channel_name);
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
  boost::property_tree::ptree reply_tree(send_json("sendMessage", tree));
  if(reply_tree.get("ok", "") != "true") {
    std::cerr << "LibTelegram: Sender: Returned status other than OK in reply to send_message:" << std::endl;
    boost::property_tree::write_json(std::cerr, reply_tree);
  }
}

}

#endif // TELEGRAM_SENDER_H_INCLUDED
