/// This example demonstrates a bot that accepts two locations, and then sends
/// a live location interpolating smoothly between the two.

#include <libtelegram/libtelegram.h>

auto main()->int {
  std::string const token("123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");
  telegram::sender sender(token);
  telegram::listener::poll listener(sender);

  listener.set_callback_message([&](telegram::types::message const &message){

  });
  listener.run();                                                               // launch the listener - this call blocks until interrupted
  return EXIT_SUCCESS;
};
