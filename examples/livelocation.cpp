/// This example demonstrates a bot that accepts two locations, and then sends
/// a live location interpolating smoothly between the two.
/// As this is a very simple example, it does not keep track of multiple users,
/// so should only be used by one user at a time.
/// It could be extended to support multiple users by tracking each user's
/// status along with user ID, with a separate thread running to update the live
/// locations for each.

#include <libtelegram/libtelegram.h>

auto main()->int {
  std::string const token("123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");
  telegram::sender sender(token);
  telegram::listener::poll listener(sender);

  std::optional<telegram::types::location> first_location;                      // cache our first location, when it arrives
  std::optional<telegram::types::location> second_location;                     // cache our second location, when it arrives

  listener.set_callback_message([&](telegram::types::message const &message){
    auto message_chat_id = message.chat.id;
    if(!first_location) {                                                       // we haven't received any locations yet from the user
      if(!message.location) {                                                   // early exit if this message doesn't contain a location
        sender.send_message(message_chat_id, "Send me the first location.");
        return;
      }
      first_location = message.location;                                        // cache the first location
      sender.send_message(message_chat_id,
                          "Interpolating from " +
                          std::to_string(first_location->longitude) + ", " +
                          std::to_string(first_location->latitude) + ".\n"
                          "Send me the second location.");
    } else {                                                                    // we already received the first location, so we want the second
      if(!message.location) {                                                   // early exit if this message doesn't contain a location
        sender.send_message(message_chat_id, "Send me the second location.");
        return;
      }
      second_location = message.location;                                       // cache the second location
      sender.send_message(message_chat_id,
                          "Interpolating from " +
                          std::to_string(first_location->longitude) + ", " +
                          std::to_string(first_location->latitude) + " to " +
                          std::to_string(second_location->longitude) + ", " +
                          std::to_string(second_location->latitude) + "...\n");
      // TODO: loop here
    }
  });
  listener.set_num_threads(1);                                                  // run single-threaded
  listener.run();                                                               // launch the listener - this call blocks until interrupted
  return EXIT_SUCCESS;
};
