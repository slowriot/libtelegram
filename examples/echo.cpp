#include  <cstdlib>
#include "libtelegram/libtelegram.h"

auto main()->int {
  std::string const token("placeholder");

  telegram::listener listener;
  listener.register_callback([](std::string const &input){
    std::cerr << "DEBUG: callback called with " << input << std::endl;
  });
  listener.run();

  telegram::sender sender("token");

  return EXIT_SUCCESS;
};
