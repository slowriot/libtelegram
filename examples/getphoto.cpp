/// This example demonstrates a bot that accepts photos, analyses them and
/// presents information to the user, then downloads those photos to disk.
/// It also demonstrates how to use optional types safely to deal with
/// unexpected errors, and how to send chat actions.
/// It also shows how to switch to single-threaded mode.

#include <libtelegram/libtelegram.h>

auto main()->int {
  std::string const token("123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");
  telegram::sender sender(token);
  telegram::listener::poll listener(sender);

  auto to_kb = [](auto value){                                                  // helper lambda to convert values in bytes to kilobytes
    std::stringstream ss;
    ss << static_cast<float>(value) / 1024 << "KB";
    return ss.str();
  };

  listener.set_callback_message([&](telegram::types::message const &message){
    auto message_chat_id = message.chat.id;
    if(!message.photo) {                                                        // check if this message contains a list of photo sizes
      if(message.text && *message.text == "/start") {                           // if it doesn't, and they've asked to /start, give them help
        sender.send_message(message_chat_id, "Send me a photo.");
      }
      return;
    }
    auto const &photo_sizes(*message.photo);                                    // if it does, dereference the optional for later use
    sender.send_message(message_chat_id, std::string("I received a photo in the ") + std::to_string(photo_sizes.size()) + " following sizes:");
    unsigned int counter = 0;
    for(auto const &photo : photo_sizes) {                                      // iterate through the vector we've received
      ++counter;
      std::stringstream ss;
      ss << "Photo " << counter << " of " << photo_sizes.size()
         << ": size: " << photo.width << "x" << photo.height;                   // tell the user what we know about the photos
      if(photo.file_size) {
        ss << ", " << to_kb(*photo.file_size) << '\n';                          // use the kilobyte conversion lambda we made earlier
      } else {
        ss << ", size unknown" << '\n';
      }
      ss << "file_id: " << photo.file_id;
      sender.send_message(message_chat_id, ss.str());

      sender.send_chat_action(message_chat_id, telegram::sender::chat_action_type::TYPING); // show the user they should wait, we're typing
      auto const &file_optional(sender.get_file(photo.file_id));                // pass the photo's file_id to the get_file method
      if(!file_optional) {                                                      // we might not get a file back, so handle this gracefully
        sender.send_message(message_chat_id, "Something went wrong and I couldn't download the file.");
        return;
      }
      auto const &file(*file_optional);
      ss.str({});                                                               // clear and reuse the stringstream
      if(file.file_path) {
        ss << "file_path: " << *file.file_path << '\n';                         // this is the file path on the Telegram servers
      } else {
        ss << "file_path unknown" << '\n';                                      // file path is optional, so we need to handle the case where it's missing
      }
      if(file.file_size) {
        ss << "file_size: " << to_kb(*file.file_size) << '\n';                  // use the kilobyte conversion lambda we made earlier
      } else {
        ss << "file_size unknown" << '\n';                                      // file size is optional, so we need to handle the case where it's missing
      }
      //ss << "URL: " << file.get_url(token).to_string() << '\n';                 // NOTE: this contains your bot's token, so be careful!  Uncomment it for testing only.
      ss << "Downloading now...";
      sender.send_message(message_chat_id, ss.str());

      sender.send_chat_action(message_chat_id, telegram::sender::chat_action_type::RECORD_VIDEO); // show the user they should wait with a different action for a change
      auto file_blob(file.download(token));                                     // grab the file into a vector<char> in memory, with which we can do whatever we want
      ss.str({});
      ss << "Downloaded, file actual size: " << to_kb(file_blob.size());
      sender.send_message(message_chat_id, ss.str());
      std::string const filename(std::string("photo_" + file.file_id + ".jpg")); // give it a unique filename
      std::ofstream filestream(std::string("photo_" + file.file_id + ".jpg"), std::ios::binary); // make sure we write in binary mode
      ss.str({});
      filestream.write(file_blob.data(), file_blob.size());                     // write the binary blob to disk
      if(filestream.fail()) {                                                   // check if the write failed
        ss << "Failed to write to \"" << filename << "\"";
        std::cerr << "Error: could not write to \"" << filename << "\"" << std::endl; // also report the failure in the webserver log - it's probably a directory permissions issue
      } else {
        ss << "Wrote " << to_kb(filestream.tellp()) << " to \"" << filename << "\""; // if successful, report how much we wrote
      }
      sender.send_message(message_chat_id, ss.str());
    }
  });
  listener.set_num_threads(1);                                                  // run single-threaded to avoid thrashing the filesystem under heavy use
  listener.run();                                                               // launch the listener - this call blocks until the fastcgi server drops us
  return EXIT_SUCCESS;
};
