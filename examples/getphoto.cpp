/// This example demonstrates a bot that accepts photos, analyses them and
/// presents information to the user, then downloads those photos to disk.
/// It also demonstrates how to use optional types safely to deal with
/// unexpected errors, and how to send chat actions.

#include "libtelegram/libtelegram.h"

auto main()->int {
  std::string const token("bot123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");
  telegram::listener listener;
  telegram::sender sender(token);
  listener.set_callback_message([&](telegram::types::message const &message){
    auto message_chat_id = message.chat.id;
    if(!message.photo) {
      if(message.text == "/start") {
        sender.send_message(message_chat_id, "Send me a photo.");
      }
      return;
    }
    auto const &photo_sizes(*message.photo);
    sender.send_message(message_chat_id, std::string("I received a photo in the ") + std::to_string(photo_sizes.size()) + " following sizes:");
    unsigned int counter = 0;
    for(auto const &photo : photo_sizes) {
      ++counter;
      std::stringstream ss;
      ss << "Photo " << counter << " of " << photo_sizes.size()
         << ": size: " << photo.width << "x" << photo.height
         << ", " << static_cast<float>(photo.file_size) / 1024 << "KB" << '\n'
         << "file_id: " << photo.file_id;
      sender.send_message(message_chat_id, ss.str());

      sender.send_chat_action(message_chat_id, telegram::sender::chat_action_type::TYPING);
      auto const &file_optional(sender.get_file(photo.file_id));
      if(!file_optional) {
        sender.send_message(message_chat_id, "Something went wrong and I couldn't download the file.");
        return;
      }
      auto const &file(*file_optional);
      ss.str({});
      ss << "file_path: " << file.file_path << '\n'
         << "file_size: " << static_cast<float>(file.file_size) / 1024 << "KB" << '\n';
      //ss << "URL: " << file.get_url(token).to_string() << '\n'; // NOTE: this contains your bot's token, so be careful!
      ss << "Downloading now...";
      sender.send_message(message_chat_id, ss.str());

      sender.send_chat_action(message_chat_id, telegram::sender::chat_action_type::RECORD_VIDEO);
      auto file_blob(file.download(token));
      ss.str({});
      ss << "Downloaded, file actual size: " << static_cast<float>(file_blob.size()) / 1024 << "KB";
      sender.send_message(message_chat_id, ss.str());
    }
  });
  listener.set_num_threads(8);                                                  // configure how many threads to run with
  listener.run();                                                               // launch the listener - this call blocks until the fastcgi server drops us
  return EXIT_SUCCESS;
};
