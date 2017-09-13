/// This example demonstrates a bot that uploads binary data in various formats

#include <libtelegram/libtelegram.h>

auto main()->int {
  std::string const token("123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");
  telegram::sender sender(token);
  telegram::listener::poll listener(sender);
  listener.set_callback_json([](nlohmann::json const &input){
    // DEBUG ONLY:
    std::cerr << input.dump(2) << std::endl;
  });
  listener.set_callback_message_json([&](nlohmann::json const &input){
    auto const &message(telegram::types::message::from_json(input));

    // TODO

    return;
  });
  listener.set_num_threads(1);                                                  // configure how many threads to run with
  listener.run();                                                               // launch the listener - this call blocks until the fastcgi server drops us
  return EXIT_SUCCESS;
};
