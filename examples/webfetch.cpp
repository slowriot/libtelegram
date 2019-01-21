/// This example demonstrates a bot that takes URLs from messages, fetches them
/// and displays their content in a reply to the user.
/// It also demonstrates more advanced message sending parameters.
/// This uses libvoxelstorm's automatic message splitting, for easily posting
/// long messages.

#include <libtelegram/libtelegram.h>

auto main()->int {
  std::string const token("123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");
  telegram::sender sender(token);
  telegram::listener::poll listener(sender);
  listener.set_callback_message([&](telegram::types::message const &message){
    if(!message.text) {
      return;                                                                   // don't try to deal with empty messages
    }
    urdl::url url;
    try {
      // try to parse the text to see if it's a URL
      url = *message.text;                                                      // this throws if a valid URL cannot be created
    } catch(std::exception const &e) {
      sender.send_message(message.chat.id,                                      // the chat to reply to
                          std::string("That doesn't appear to be a valid URL: ") + e.what(), // text of the message to send including the exception
                          message.message_id);                                  // reply to the sender's message id
      return;
    }
    try {
      std::shared_ptr<httplib::Response> http_result;
      if(url.protocol() == "http") {
        httplib::Client http_client(url.host().c_str(), url.port(), 10);
        http_result = http_client.Get((url.path() + "?" + url.query()).c_str());
      } else if(url.protocol() == "https") {
        httplib::SSLClient https_client(url.host().c_str(), url.port(), 10);
        http_result = https_client.Get((url.path() + "?" + url.query()).c_str());
      } else {
        sender.send_message(message.chat.id,                                    // the chat to reply to
                            "Unknown protocol " + url.protocol(),               // text of the message to send including the URDL error message
                            message.message_id);                                // reply to the sender's message id
        return;
      }
      if(!http_result || http_result->status != 200) {
        sender.send_message(message.chat.id,                                    // the chat to reply to
                            std::string("Error when opening that URL: ") + std::to_string(http_result->status), // text of the message to send including the URDL error message
                            message.message_id);                                // reply to the sender's message id
        return;
      }
      sender.send_message(message.chat.id,                                      // the chat to reply to
                          http_result->body,                                    // send the complete body
                          message.message_id,                                   // reply to the sender's message id
                          telegram::sender::parse_mode::HTML,                   // attempt to render HTML elements Telegram supports, see https://core.telegram.org/bots/api#html-style
                          telegram::sender::web_preview_mode::DISABLE,          // don't allow URLs to produce previews
                          telegram::sender::notification_mode::DEFAULT);        // leave notifications at the API default mode
    } catch(std::exception const &e) {
      sender.send_message(message.chat.id,                                      // the chat to reply to
                          std::string("Exception when opening that URL: ") + e.what(), // text of the message to send including the thrown exception
                          message.message_id);                                  // reply to the sender's message id
    }
  });
  listener.run();
  return EXIT_SUCCESS;
};
