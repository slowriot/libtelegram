#include  <cstdlib>
#include "libtelegram/libtelegram.h"

auto main()->int {
  std::string const token("229029247:AAHfNUvcGS_ttvvulZG3Qi4NmMrjebH8F6w");

  telegram::listener listener;
  telegram::sender sender("token");
  listener.set_callback_raw([](std::string const &input){
    std::cerr << "DEBUG: raw callback called with " << input << std::endl;
  });
  listener.set_callback_json([](boost::property_tree::ptree const &input){
    std::cerr << "DEBUG: json callback called with update id " << input.get("update_id", "unknown") << std::endl;
  });
  listener.set_callback_message([](boost::property_tree::ptree const &input){
    std::cerr << "DEBUG: message callback called with message \"" << input.get("text", "") << "\"" <<
                 " from " << input.get("from.first_name", "unknown") <<
                 " in chat id " << input.get("chat.id", "unknown") << std::endl;
  });
  listener.run();

  return EXIT_SUCCESS;
};
