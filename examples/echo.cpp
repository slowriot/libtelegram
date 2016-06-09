#include  <cstdlib>
#include "libtelegram/libtelegram.h"

auto main()->int {
  std::string const token("229029247:AAHfNUvcGS_ttvvulZG3Qi4NmMrjebH8F6w");
  telegram::listener listener;
  telegram::sender sender(token);
  listener.set_callback_message([&](boost::property_tree::ptree const &input){
    int_fast64_t const message_chat_id(input.get<int_fast64_t>("chat.id", -1));
    std::string  const message_sender( input.get("from.first_name", "unknown"));
    std::string  const message_text(   input.get("text", ""));
    sender.send_message(message_chat_id, message_sender + " sent \"" + message_text + "\" to chat id " + std::to_string(message_chat_id));
  });
  listener.run();

  return EXIT_SUCCESS;
};
