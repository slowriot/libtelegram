/// This example demonstrates a minimal bot that just echoes back what it reads.
/// This is a version of the echo example that, instead of using built-in
/// objects, accesses the property tree directly.  It's a bit more verbose but
/// potentially far more powerful.

#include <libtelegram/libtelegram.h>

auto main()->int {
  std::string const token("123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");         // in practice you would probably read this from config file or database
  telegram::sender sender(token);                                               // create a sender with our token for outgoing messages
  telegram::listener::poll listener(sender);                                    // create a polling listener which will process incoming requests, polling using the sender
  listener.set_callback_message_json([&](nlohmann::json const &input){          // we set a callback for receiving messages in json format, using a lambda for convenience
    std::string  const message_text(   input.at("text").get<std::string>());    // message components are passed in json objects, and can be obtained with bounds checking
    std::string  const message_sender( input["from"]["first_name"].get<std::string>()); // we can use operator[] and chain properties for access
    int_fast64_t const message_chat_id(input["chat"]["id"]);                    // the return type can be deduced automatically in many cases
    sender.send_message(message_chat_id, message_sender + " sent \"" + message_text + "\" to chat id " + std::to_string(message_chat_id)); // send our reply from within the callback
  });
  listener.run();                                                               // launch the listener - this call blocks until the fastcgi server drops us
  return EXIT_SUCCESS;
};
