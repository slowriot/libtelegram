/// This example demonstrates a bot that takes provides a custom keyboard and
/// processes the results of pressing those keys.

#include <libtelegram/libtelegram.h>

auto main()->int {
  std::string const token("123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11");
  telegram::sender sender(token);
  telegram::listener::poll listener(sender);
  listener.set_callback_message_json([&](nlohmann::json const &input){
    auto const &message(telegram::types::message::from_json(input));

    if(message.text && *message.text == "/start") {                             // if they've asked us to start, send them a keyboard
      telegram::types::reply_markup::reply_keyboard_markup reply_markup;
      reply_markup.keyboard_buttons.emplace_back();
      reply_markup.keyboard_buttons.back().emplace_back("7");
      reply_markup.keyboard_buttons.back().emplace_back("8");
      reply_markup.keyboard_buttons.back().emplace_back("9");
      reply_markup.keyboard_buttons.back().emplace_back("/");
      reply_markup.keyboard_buttons.emplace_back();
      reply_markup.keyboard_buttons.back().emplace_back("4");
      reply_markup.keyboard_buttons.back().emplace_back("5");
      reply_markup.keyboard_buttons.back().emplace_back("6");
      reply_markup.keyboard_buttons.back().emplace_back("x");
      reply_markup.keyboard_buttons.emplace_back();
      reply_markup.keyboard_buttons.back().emplace_back("1");
      reply_markup.keyboard_buttons.back().emplace_back("2");
      reply_markup.keyboard_buttons.back().emplace_back("3");
      reply_markup.keyboard_buttons.back().emplace_back("+");
      reply_markup.keyboard_buttons.emplace_back();
      reply_markup.keyboard_buttons.back().emplace_back(".");
      reply_markup.keyboard_buttons.back().emplace_back("0");
      reply_markup.keyboard_buttons.back().emplace_back("=");
      reply_markup.keyboard_buttons.back().emplace_back("-");
      sender.send_message(message.chat.id,
                          "Calculator is ready.",
                          reply_markup);
    }
    return;

  });
  listener.set_num_threads(1);                                                  // configure how many threads to run with
  listener.run();                                                               // launch the listener - this call blocks until the fastcgi server drops us
  return EXIT_SUCCESS;
};
