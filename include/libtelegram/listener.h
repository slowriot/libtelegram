#ifndef TELEGRAM_LISTENER_H_INCLUDED
#define TELEGRAM_LISTENER_H_INCLUDED

#include <functional>
#include <iostream>
#include <thread>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#define BOOST_CGI_NO_BOOST_FILESYSTEM
#include <boost/cgi/fcgi.hpp>

namespace telegram {

class listener {
public:
  static unsigned int constexpr const thread_number_chosen_automatically = 0;

private:
  std::function<void(std::string const&)>                 callback_raw            = [](std::string                 const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_json           = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_message        = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_edited         = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_inline         = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_chosen_inline  = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_callback       = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};

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
  //   raw boost::fcgi::request &request object

  unsigned int num_threads = thread_number_chosen_automatically;

public:
  listener();

  void set_callback_raw(          std::function<void(std::string                 const &input)> func);
  void set_callback_json(         std::function<void(boost::property_tree::ptree const &input)> func);
  void set_callback_message(      std::function<void(boost::property_tree::ptree const &input)> func);
  void set_callback_edited(       std::function<void(boost::property_tree::ptree const &input)> func);
  void set_callback_inline(       std::function<void(boost::property_tree::ptree const &input)> func);
  void set_callback_chosen_inline(std::function<void(boost::property_tree::ptree const &input)> func);
  void set_callback_callback(     std::function<void(boost::property_tree::ptree const &input)> func);

  void unset_callback_raw();
  void unset_callback_json();
  void unset_callback_message();
  void unset_callback_edited();
  void unset_callback_inline();
  void unset_callback_chosen_inline();
  void unset_callback_callback();
  void unset_callbacks();

  unsigned int get_num_threads();
  void set_num_threads(unsigned int new_num_threads = thread_number_chosen_automatically);

  void run();

private:
  int handle_request(boost::fcgi::request &request);
  int handle_request_async(boost::fcgi::acceptor &acceptor, boost::fcgi::request &request);
};

listener::listener() {
  if(num_threads == thread_number_chosen_automatically) {
    set_num_threads();                                                          // update the thread count to the default
  }
}

void listener::set_callback_raw(std::function<void(std::string const &input)> func) {
  /// Set a callback to receive the complete raw data, if you want to process the json yourself
  callback_raw = func;
}
void listener::set_callback_json(std::function<void(boost::property_tree::ptree const &input)> func) {
  /// Set a callback to receive the complete processed json in a boost property tree, if you want to pull out custom data manually
  callback_json = func;
}
void listener::set_callback_message(std::function<void(boost::property_tree::ptree const &input)> func) {
  /// Set a callback to receive any messages in property tree format
  callback_message = func;
}
void listener::set_callback_edited(std::function<void(boost::property_tree::ptree const &input)> func) {
  /// Set a callback to receive the new versions of edited messages in property tree format
  callback_edited = func;
}
void listener::set_callback_inline(std::function<void(boost::property_tree::ptree const &input)> func) {
  /// Set a callback to receive inline queries in property tree format
  callback_einline = func;
}
void listener::set_callback_chosen_inline(std::function<void(boost::property_tree::ptree const &input)> func) {
  /// Set a callback to receive the result of an inline query that was chosen by a user and sent to their chat partner in property tree format
  callback_chosen_inline = func;
}
void listener::set_callback_callback(std::function<void(boost::property_tree::ptree const &input)> func) {
  /// Set a callback to receive new incoming callback queries in property tree format
  callback_callback = func;
}

void listener::unset_callback_raw() {
  /// Helper to unset this callback
  callback_raw = nullptr;
}
void listener::unset_callback_json() {
  /// Helper to unset this callback
  callback_json = nullptr;
}
void listener::unset_callback_message() {
  /// Helper to unset this callback
  callback_message = nullptr;
}
void listener::unset_callback_edited() {
  /// Helper to unset this callback
  callback_edited = nullptr;
}
void listener::unset_callback_inline() {
  /// Helper to unset this callback
  callback_inline = nullptr;
}
void listener::unset_callback_chosen_inline() {
  /// Helper to unset this callback
  callback_chosen_inline = nullptr;
}
void listener::unset_callback_callback() {
  /// Helper to unset this callback
  callback_callback = nullptr;
}
void listener::unset_callbacks() {
  /// Helper function to unset all callbacks at once
  callback_raw            = nullptr;
  callback_json           = nullptr;
  callback_message        = nullptr;
  callback_edited         = nullptr;
  callback_inline         = nullptr;
  callback_chosen_inline  = nullptr;
  callback_callback       = nullptr;
}

unsigned int listener::get_num_threads() {
  /// Report how many threads we're using to execute acceptors
  return num_threads;
}
void listener::set_num_threads(unsigned int new_num_threads) {
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

void listener::run() {
  /// Execute the telegram service
  try {
    boost::fcgi::service service;
    boost::fcgi::acceptor acceptor(service);
    for(unsigned int i = 0; i != 2; ++i) {
      acceptor.async_accept(boost::bind(&telegram::listener::handle_request_async, this, boost::ref(acceptor), _1));
    }
    if(num_threads == 1) {
      service.run();                                                            // single-threaded operation runs the service on the main thread
    } else {
      std::vector<std::thread> threads;
      for(unsigned int i = 0; i != num_threads; ++i) {                          // run however many threads we've requested
        threads.emplace_back([&service]{
          service.run();
        });
      }
      for(auto &thread : threads) {                                             // the main thread idles, waiting for the child threads to finish
        thread.join();
      }
    }
  } catch(boost::system::system_error const &se) {                              // this is the type of error thrown by the fcgi library.
    std::cerr << "LibTelegram: System error: " << se.what() << std::endl;
  } catch(std::exception const &e) {                                            // catch any other exceptions
    std::cerr << "LibTelegram: Exception: " << e.what() << std::endl;
  } catch(...) {
    std::cerr<< "LibTelegram: Uncaught exception!" << std::endl;
  }
}

int listener::handle_request(boost::fcgi::request &request) {
  /// Request handler - parse what we received and dispatch callbacks appropriately
  std::cerr<< "LibTelegram: DEBUG: started handling a request" << std::endl;
  boost::system::error_code error;
  request.load(boost::fcgi::parse_form, error);                                 // see boost/cgi/common/parse_options.hpp
  boost::fcgi::response response;
  if(error) {
    std::cerr << "LibTelegram: Request parsing error: " << error.message() << std::endl;
    response << "Request parsing error: " << error.message();
    return boost::fcgi::commit(request, response, boost::fcgi::http::internal_server_error);
  }
  if(request.method() != "POST") {                                              // is this a post response?
    response << boost::fcgi::content_type("text/html");                         // set a content type for the normal content output
    response << "Bad request.";                                                 // this probably didn't come from Telegram, so don't give away any additional information
    return boost::fcgi::commit(request, response, boost::fcgi::http::bad_request);
  }

  // deal with the data
  if(callback_raw) {
    callback_raw(request.post_buffer());                                        // if the raw callback is set, send the whole buffer straight there
  }
  boost::property_tree::ptree tree;                                             // create empty property tree
  try {
    boost::iostreams::stream<boost::iostreams::array_source> stream(request.post_buffer().data(), request.post_buffer().size()); // wrap the request buffer in a stream, avoiding a copy unlike stringstream
    boost::property_tree::read_json(stream, tree);                              // parse the json into the property tree
  } catch(std::exception &e) {
    response << boost::fcgi::content_type("text/html");                         // set a content type for the normal content output
    std::cerr << "LibTelegram: Received unparseable JSON: " << e.what() << std::endl;
    response << "Received unparseable JSON";                                    // if we got this far, complain what exactly fucked up
    return boost::fcgi::commit(request, response, boost::fcgi::http::bad_request);
  }
  // the json must be fine, so generate a reply
  response << boost::fcgi::content_type("text/html");                           // set a content type for the normal content output
  response << "OK";
  if(callback_json) {
    callback_json(tree);                                                        // if the json callback is set, send the whole tree
  }
  if(callback_message) {                                                        // only check for a message if we've got a callback set
    try {
      callback_message(tree.get_child("message"));
    } catch(std::exception &e) {}                                               // this update doesn't include a message - no problem, carry on
  }
  if(callback_edited) {                                                         // only check for an edited message if we've got a callback set
    try {
      callback_edited(tree.get_child("edited_message"));
    } catch(std::exception &e) {}                                               // this update doesn't include an edited message - no problem, carry on
  }
  if(callback_inline) {                                                         // only check for an inline query if we've got a callback set
    try {
      callback_edited(tree.get_child("inline_query"));
    } catch(std::exception &e) {}                                               // this update doesn't include an inline query - no problem, carry on
  }
  if(callback_chosen_inline) {                                                  // only check for a chosen inline result if we've got a callback set
    try {
      callback_edited(tree.get_child("chosen_inline_result"));
    } catch(std::exception &e) {}                                               // this update doesn't include a chosen inline result - no problem, carry on
  }
  if(callback_callback) {                                                       // only check for a callback query if we've got a callback callback set
    try {
      callback_edited(tree.get_child("callback_query"));
    } catch(std::exception &e) {}                                               // this update doesn't include a callback query - no problem, carry on
  }

  return boost::fcgi::commit(request, response, boost::fcgi::http::ok);         // commit the response and obtain the result
}

int listener::handle_request_async(boost::fcgi::acceptor &acceptor, boost::fcgi::request &request) {
  /// Asynchronous request handler wrapper - restarts itself after running
  int const result = handle_request(request);
  acceptor.async_accept(boost::bind(&telegram::listener::handle_request_async, this, boost::ref(acceptor), _1)); // restart the acceptor
  return result;
}

}

#endif // TELEGRAM_LISTENER_H_INCLUDED
