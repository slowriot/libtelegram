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

  auto lerp = [](float from, float to, float coefficient){                      // helper function to linearly interpolate
    return from + coefficient * (to - from);
  };

  std::optional<telegram::types::location> first_location;                      // cache our first location, when it arrives
  std::optional<telegram::types::location> second_location;                     // cache our second location, when it arrives

  listener.set_callback_message([&](telegram::types::message const &message){
    auto const message_chat_id = message.chat.id;
    if(!first_location) {                                                       // we haven't received any locations yet from the user
      if(!message.location) {                                                   // early exit if this message doesn't contain a location
        sender.send_message(message_chat_id, "Send me the first location.");
        return;
      }
      first_location = message.location;                                        // cache the first location
      sender.send_message(message_chat_id,
                          "Interpolating from " +
                          std::to_string(first_location->latitude) + ", " +
                          std::to_string(first_location->longitude) + ".\n"
                          "Send me the second location.");
    } else {                                                                    // we already received the first location, so we want the second
      if(!message.location) {                                                   // early exit if this message doesn't contain a location
        sender.send_message(message_chat_id, "Send me the second location.");
        return;
      }
      second_location = message.location;                                       // cache the second location
      sender.send_message(message_chat_id,
                          "Interpolating from " +
                          std::to_string(first_location->latitude) + ", " +
                          std::to_string(first_location->longitude) + " to " +
                          std::to_string(second_location->latitude) + ", " +
                          std::to_string(second_location->longitude) + "...\n");

      unsigned int constexpr const live_location_period = 60;                   // time in seconds
      unsigned int constexpr const seconds_per_update = 2;                      // don't flood with updates too frequently

      auto const location_message = sender.send_location(message_chat_id,       // send the initial location first, we'll then update it.  This function returns the message we sent, we'll need its id to update it
                                                         *first_location,
                                                         live_location_period); // minimum 60, maximum 86400

      if(!location_message || !location_message->location) {                    // error checking: make sure we got back a message with a correctly sent location
        sender.send_message(message_chat_id, "Something went wrong - unable to send back that first location!");
        return;
      }
      auto sent_message_id = location_message->message_id;                      // this is the message id of the location message we're updating

      unsigned int constexpr const steps = live_location_period / seconds_per_update;
      for(unsigned int step = 0; step != steps; ++step) {
        float coefficient = static_cast<float>(step) / steps;
        telegram::types::location const interpolated_location{
          lerp(first_location->latitude,  second_location->latitude,  coefficient),
          lerp(first_location->longitude, second_location->longitude, coefficient)
        };

        std::cout << "DEBUG: updating message id " << sent_message_id << std::endl;
        auto const updated_message = sender.edit_message_live_location(message_chat_id, // update the live location - we need both the chat id and...
                                                                       sent_message_id, // ...the message id we're updating
                                                                       interpolated_location);
        //sent_message_id = updated_message->message_id;

        using namespace std::chrono_literals;                                   // to let us use the "seconds" literal
        std::this_thread::sleep_for(1s * seconds_per_update);                   // sleep this thread until it's time to send the next message
      }
    }
  });
  listener.set_num_threads(1);                                                  // run single-threaded
  listener.run();                                                               // launch the listener - this call blocks until interrupted
  return EXIT_SUCCESS;
};
