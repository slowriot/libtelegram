#include  <cstdlib>
#include "libtelegram/libtelegram.h"

auto main()->int {
  std::string const token("placeholder");

  telegram::listener listener;
  listener.register_callback_raw([](std::string const &input){
    std::cerr << "DEBUG: raw callback called with " << input << std::endl;
  });
  listener.register_callback_json([](boost::property_tree::ptree const &input){
    std::cerr << "DEBUG: json callback called with update id " << input.get("update_id", "unknown") << std::endl;
  });
  listener.run();

  telegram::sender sender("token");

  return EXIT_SUCCESS;
};
