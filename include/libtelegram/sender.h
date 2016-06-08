#ifndef TELEGRAM_SENDER_H_INCLUDED
#define TELEGRAM_SENDER_H_INCLUDED

#include <string>

namespace telegram {

class sender {
  std::string token;

public:
  enum class parse_mode : char {
    NONE,
    MARKDOWN,
    HTML
  };
  enum class web_preview_mode : bool {
    DISABLE,
    ENABLE
  };
  enum class notification_mode : bool {
    DISABLE,
    ENABLE
  };

  sender(std::string const &token);

  void send_message(int_fast64_t chat_id,
                    std::string const &text,
                    parse_mode parse_mode = parse_mode::NONE,
                    web_preview_mode web_preview = web_preview_mode::ENABLE,
                    notification_mode notification = notification_mode::ENABLE,
                    int_fast32_t reply_to_message_id = -1);
  void send_message(std::string channel_name,
                    std::string const &text,
                    parse_mode parse_mode = parse_mode::NONE,
                    web_preview_mode web_preview = web_preview_mode::ENABLE,
                    notification_mode notification = notification_mode::ENABLE,
                    int_fast32_t reply_to_message_id = -1);
};

sender::sender(std::string const &this_token)
  : token(this_token) {
  /// Construct a sender with the given token
}

void sender::send_message(int_fast64_t chat_id,
                          std::string const &text,
                          parse_mode parse_mode,
                          web_preview_mode web_preview,
                          notification_mode notification,
                          int_fast32_t reply_to_message_id) {
  /// Send a message to a chat id
  std::cerr << "DEBUG: sending message \"" << text << "\" to chat id " << chat_id << std::endl;

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
