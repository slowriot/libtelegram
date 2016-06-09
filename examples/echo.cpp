#include  <cstdlib>
#include "libtelegram/libtelegram.h"

auto main()->int {
  std::string const token("229029247:AAHfNUvcGS_ttvvulZG3Qi4NmMrjebH8F6w");
  telegram::listener listener;
  telegram::sender sender(token);
  listener.set_callback_raw([](std::string const &input){
    std::cerr << "DEBUG: raw callback called with " << input << std::endl;
  });
  listener.set_callback_json([](boost::property_tree::ptree const &input){
    std::cerr << "DEBUG: json callback called with update id " << input.get("update_id", "unknown") << std::endl;
  });
  listener.set_callback_message([&](boost::property_tree::ptree const &input){
    int_fast64_t const message_chat_id(input.get<int_fast64_t>("chat.id", -1));
    std::string  const message_sender( input.get("from.first_name", "unknown"));
    std::string  const message_text(   input.get("text", ""));
    std::cerr << "DEBUG: message callback called with message \"" << message_text << "\"" <<
                 " from " << message_sender <<
                 " in chat id " << message_chat_id << std::endl;
    sender.send_message(message_chat_id, message_sender + " sent \"" + message_text + "\" to chat id " + std::to_string(message_chat_id));
  });
  listener.run();

  return EXIT_SUCCESS;
};
