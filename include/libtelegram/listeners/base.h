#ifndef TELEGRAM_LISTENERS_BASE_H_INCLUDED
#define TELEGRAM_LISTENERS_BASE_H_INCLUDED

#include <thread>
#include "libtelegram/types/types.h"

namespace telegram::listener {

template<typename T>
class base {
  /// CRTP style static polymorphic base class for listeners
protected:
  std::function<void(std::string                 const&)> callback_raw                 = [](std::string                 const &input __attribute__((__unused__))){};
  std::function<void(nlohmann::json              const&)> callback_json                = [](nlohmann::json              const &input __attribute__((__unused__))){};
  std::function<void(types::message              const&)> callback_message             = [](types::message              const &input __attribute__((__unused__))){};
  std::function<void(nlohmann::json              const&)> callback_message_json        = [](nlohmann::json              const &input __attribute__((__unused__))){};
  std::function<void(types::message              const&)> callback_edited              = [](types::message              const &input __attribute__((__unused__))){};
  std::function<void(nlohmann::json              const&)> callback_edited_json         = [](nlohmann::json              const &input __attribute__((__unused__))){};
  //std::function<void(types::inline_query         const&)> callback_inline              = [](types::inline_query         const &input __attribute__((__unused__))){};
  std::function<void(nlohmann::json              const&)> callback_inline_json         = [](nlohmann::json              const &input __attribute__((__unused__))){};
  //std::function<void(types::chosen_inline_result const&)> callback_chosen_inline       = [](types::chosen_inline_result const &input __attribute__((__unused__))){};
  std::function<void(nlohmann::json              const&)> callback_chosen_inline_json  = [](nlohmann::json              const &input __attribute__((__unused__))){};
  std::function<void(types::callback_query       const&)> callback_query_callback      = [](types::callback_query       const &input __attribute__((__unused__))){};
  std::function<void(nlohmann::json              const&)> callback_query_callback_json = [](nlohmann::json              const &input __attribute__((__unused__))){};

  // TODO:
  // instance start timestamp
  // thread stats
  //   number & percent of requests processed
  //   min, avg, max time per request
  // total requests processed
  // min, avg, max time per request
  // min, avg, max time taken to process each callback type
  //
  // listener data structure:
  //   thread id
  //   raw boost::base<T>::request &request object

public:
  static unsigned int constexpr const thread_number_chosen_automatically = 0;
protected:
  unsigned int num_threads = thread_number_chosen_automatically;

public:
  base();

  void set_callback_raw(                std::function<void(std::string const &input)> func);
  void set_callback_json(               std::function<void(nlohmann::json const &input)> func);
  void set_callback_message(            std::function<void(types::message const &input)> func);
  void set_callback_message_json(       std::function<void(nlohmann::json const &input)> func);
  void set_callback_edited(             std::function<void(types::message const &input)> func);
  void set_callback_edited_json(        std::function<void(nlohmann::json const &input)> func);
  //void set_callback_inline(             std::function<void(types::inline_query const &input)> func);
  void set_callback_inline_json(        std::function<void(nlohmann::json const &input)> func);
  //void set_callback_chosen_inline(      std::function<void(types::chosen_inline_result const &input)> func);
  void set_callback_chosen_inline_json( std::function<void(nlohmann::json const &input)> func);
  void set_callback_query_callback(     std::function<void(types::callback_query const &input)> func);
  void set_callback_query_callback_json(std::function<void(nlohmann::json const &input)> func);

  void unset_callback_raw();
  void unset_callback_raw_json();
  void unset_callback_raw_native();
  void unset_callback_json();
  void unset_callback_json_json();
  void unset_callback_json_native();
  void unset_callback_message();
  void unset_callback_message_json();
  void unset_callback_message_native();
  void unset_callback_edited();
  void unset_callback_edited_json();
  void unset_callback_edited_native();
  void unset_callback_inline();
  void unset_callback_inline_json();
  void unset_callback_inline_native();
  void unset_callback_chosen_inline();
  void unset_callback_chosen_inline_json();
  void unset_callback_chosen_inline_native();
  void unset_callback_query_callback();
  void unset_callback_query_callback_json();
  void unset_callback_query_callback_native();
  void unset_callbacks();

  unsigned int get_num_threads();
  void set_num_threads(unsigned int new_num_threads = thread_number_chosen_automatically);

  void run();

protected:
  void execute_callbacks(nlohmann::json const &tree);
};

template<typename T>
base<T>::base() {
  if(num_threads == thread_number_chosen_automatically) {
    set_num_threads();                                                          // update the thread count to the default
  }
}

template<typename T>
void base<T>::set_callback_raw(std::function<void(std::string const &input)> func) {
  /// Set a callback to receive the complete raw data, if you want to process the json yourself
  callback_raw = func;
}
template<typename T>
void base<T>::set_callback_json(std::function<void(nlohmann::json const &input)> func) {
  /// Set a callback to receive the complete processed json in a boost property tree, if you want to pull out custom data manually
  /// Returns the entire update - see https://core.telegram.org/bots/api#update
  callback_json = func;
}
template<typename T>
void base<T>::set_callback_message(std::function<void(types::message const &input)> func) {
  /// Set a callback to receive any messages in native format
  /// See https://core.telegram.org/bots/api#message
  callback_message = func;
}
template<typename T>
void base<T>::set_callback_message_json(std::function<void(nlohmann::json const &input)> func) {
  /// Set a callback to receive any messages in property tree format
  /// See https://core.telegram.org/bots/api#message
  callback_message_json = func;
}
template<typename T>
void base<T>::set_callback_edited(std::function<void(types::message const &input)> func) {
  /// Set a callback to receive the new versions of edited messages in native format
  /// See https://core.telegram.org/bots/api#message (as with normal messages)
  callback_edited = func;
}
template<typename T>
void base<T>::set_callback_edited_json(std::function<void(nlohmann::json const &input)> func) {
  /// Set a callback to receive the new versions of edited messages in property tree format
  /// See https://core.telegram.org/bots/api#message (as with normal messages)
  callback_edited_json = func;
}
/*
template<typename T>
void base<T>::set_callback_inline(std::function<void(types::inline_query const &input)> func) {
  /// Set a callback to receive inline queries in native format
  /// See https://core.telegram.org/bots/api#inlinequery
  callback_inline = func;
}
*/
template<typename T>
void base<T>::set_callback_inline_json(std::function<void(nlohmann::json const &input)> func) {
  /// Set a callback to receive inline queries in property tree format
  /// See https://core.telegram.org/bots/api#inlinequery
  callback_inline_json = func;
}
/*
template<typename T>
void base<T>::set_callback_chosen_inline(std::function<void(types::chosen_inline_result const &input)> func) {
  /// Set a callback to receive the result of an inline query that was chosen by a user and sent to their chat partner in native format
  /// See https://core.telegram.org/bots/api#choseninlineresult
  callback_chosen_inline = func;
}
*/
template<typename T>
void base<T>::set_callback_chosen_inline_json(std::function<void(nlohmann::json const &input)> func) {
  /// Set a callback to receive the result of an inline query that was chosen by a user and sent to their chat partner in property tree format
  /// See https://core.telegram.org/bots/api#choseninlineresult
  callback_chosen_inline_json = func;
}
template<typename T>
void base<T>::set_callback_query_callback(std::function<void(types::callback_query const &input)> func) {
  /// Set a callback to receive new incoming callback queries in native format
  /// See https://core.telegram.org/bots/api#callbackquery
  callback_query_callback = func;
}
template<typename T>
void base<T>::set_callback_query_callback_json(std::function<void(nlohmann::json const &input)> func) {
  /// Set a callback to receive new incoming callback queries in property tree format
  /// See https://core.telegram.org/bots/api#callbackquery
  callback_query_callback_json = func;
}

template<typename T>
void base<T>::unset_callback_raw() {
  /// Helper to unset this callback
  callback_raw = nullptr;
}
template<typename T>
void base<T>::unset_callback_json() {
  /// Helper to unset this callback
  callback_json = nullptr;
}
template<typename T>
void base<T>::unset_callback_message() {
  /// Helper to unset this callback
  unset_callback_message_json();
  unset_callback_message_native();
}
template<typename T>
void base<T>::unset_callback_message_json() {
  /// Helper to unset this callback
  callback_message_json = nullptr;
}
template<typename T>
void base<T>::unset_callback_message_native() {
  /// Helper to unset this callback
  callback_message = nullptr;
}
template<typename T>
void base<T>::unset_callback_edited() {
  /// Helper to unset this callback
  unset_callback_edited_json();
  unset_callback_edited_native();
}
template<typename T>
void base<T>::unset_callback_edited_json() {
  /// Helper to unset this callback
  callback_edited_json = nullptr;
}
template<typename T>
void base<T>::unset_callback_edited_native() {
  /// Helper to unset this callback
  callback_edited = nullptr;
}
template<typename T>
void base<T>::unset_callback_inline() {
  /// Helper to unset this callback
  unset_callback_inline_json();
  unset_callback_inline_native();
}
template<typename T>
void base<T>::unset_callback_inline_json() {
  /// Helper to unset this callback
  callback_inline_json = nullptr;
}
template<typename T>
void base<T>::unset_callback_inline_native() {
  /// Helper to unset this callback
  //callback_inline = nullptr;
}
template<typename T>
void base<T>::unset_callback_chosen_inline() {
  /// Helper to unset this callback
  unset_callback_chosen_inline_json();
  unset_callback_chosen_inline_native();
}
template<typename T>
void base<T>::unset_callback_chosen_inline_json() {
  /// Helper to unset this callback
  callback_chosen_inline_json = nullptr;
}
template<typename T>
void base<T>::unset_callback_chosen_inline_native() {
  /// Helper to unset this callback
  //callback_chosen_inline = nullptr;
}
template<typename T>
void base<T>::unset_callback_query_callback() {
  /// Helper to unset this callback
  unset_callback_query_callback_json();
  unset_callback_query_callback_native();
}
template<typename T>
void base<T>::unset_callback_query_callback_json() {
  /// Helper to unset this callback
  callback_query_callback_json = nullptr;
}
template<typename T>
void base<T>::unset_callback_query_callback_native() {
  /// Helper to unset this callback
  callback_query_callback = nullptr;
}
template<typename T>
void base<T>::unset_callbacks() {
  /// Helper function to unset all callbacks at once
  unset_callback_raw();
  unset_callback_json();
  unset_callback_message();
  unset_callback_edited();
  unset_callback_inline();
  unset_callback_chosen_inline();
  unset_callback_query_callback();
}

template<typename T>
unsigned int base<T>::get_num_threads() {
  /// Report how many threads we're using to execute acceptors
  return num_threads;
}
template<typename T>
void base<T>::set_num_threads(unsigned int new_num_threads) {
  /// Set the number of concurrent threads to run acceptors on.
  /// If new_num_threads is thread_number_chosen_automatically (which is 0), choose automatically based on number of logical cores available.
  if(new_num_threads == thread_number_chosen_automatically) {
    new_num_threads = std::thread::hardware_concurrency();                      // get the number of concurrent supported threads
    if(new_num_threads == 0) {
      new_num_threads = 1;
    }
  }
  num_threads = new_num_threads;
}

template<typename T>
void base<T>::run() {
  /// Execute the telegram listener service - CRTP polymorphic call
  static_cast<T*>(this)->run();
}

template<typename T>
void base<T>::execute_callbacks(nlohmann::json const &tree) {
  /// Process the data received from the server through the callbacks
  if(callback_json) {
    try {
      callback_json(tree);                                                      // if the json callback is set, send the whole tree
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling json callback: " << e.what() << std::endl;
    }
  }
  if(callback_message_json) {                                                   // only check for a message if we've got a callback set
    try {
      callback_message_json(tree.at("message"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include a message - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling json message callback: " << e.what() << std::endl;
    }
  }
  if(callback_message) {                                                        // only check for a message if we've got a callback set
    try {
      callback_message(types::message::from_json(tree, "message"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include a message - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling message callback: " << e.what() << std::endl;
    }
  }
  if(callback_edited_json) {                                                    // only check for an edited message if we've got a callback set
    try {
      callback_edited_json(tree.at("edited_message"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include an edited message - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling json edited message callback: " << e.what() << std::endl;
    }
  }
  if(callback_edited) {                                                         // only check for an edited message if we've got a callback set
    try {
      callback_edited(types::message::from_json(tree, "edited_message"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include an edited message - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling edited message callback: " << e.what() << std::endl;
    }
  }
  if(callback_inline_json) {                                                    // only check for an inline query if we've got a callback set
    try {
      callback_inline_json(tree.at("inline_query"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include an inline query - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling json inline query callback: " << e.what() << std::endl;
    }
  }
  /*
  if(callback_inline) {                                                         // only check for an inline query if we've got a callback set
    try {
      callback_inline(types::inline_query::from_json(tree, "inline_query"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include an inline query - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling inline query callback: " << e.what() << std::endl;
    }
  }
  */
  if(callback_chosen_inline_json) {                                             // only check for a chosen inline result if we've got a callback set
    try {
      callback_chosen_inline_json(tree.at("chosen_inline_result"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include a chosen inline result - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling json chosen inline result callback: " << e.what() << std::endl;
    }
  }
  /*
  if(callback_chosen_inline) {                                                  // only check for a chosen inline result if we've got a callback set
    try {
      callback_chosen_inline(types::chosen_inline_result::from_json(tree, "chosen_inline_result"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include a chosen inline result - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling chosen inline result callback: " << e.what() << std::endl;
    }
  }
  */
  if(callback_query_callback_json) {                                            // only check for a callback query if we've got a callback callback set
    try {
      callback_query_callback_json(tree.at("callback_query"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include a callback query - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling json callback query callback: " << e.what() << std::endl;
    }
  }
  if(callback_query_callback) {                                                 // only check for a callback query if we've got a callback query callback set
    try {
      callback_query_callback(types::callback_query::from_json(tree, "callback_query"));
    } catch(std::out_of_range &e) {                                             // this update doesn't include a callback query - no problem, carry on
    } catch(std::exception &e) {
      std::cerr << "LibTelegram: Exception calling callback query callback: " << e.what() << std::endl;
    }
  }
}

}

#endif // TELEGRAM_LISTENERS_BASE_H_INCLUDED
