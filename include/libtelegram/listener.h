#ifndef TELEGRAM_LISTENER_H_INCLUDED
#define TELEGRAM_LISTENER_H_INCLUDED

#include <functional>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#define BOOST_CGI_NO_BOOST_FILESYSTEM
#include <boost/cgi/fcgi.hpp>

namespace telegram {

class listener {
  std::function<void(std::string const&)>                 callback_raw     = [](std::string                 const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_json    = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};
  std::function<void(boost::property_tree::ptree const&)> callback_message = [](boost::property_tree::ptree const &input __attribute__((__unused__))){};

public:
  void set_callback_raw(    std::function<void(std::string                 const &input)> func);
  void set_callback_json(   std::function<void(boost::property_tree::ptree const &input)> func);
  void set_callback_message(std::function<void(boost::property_tree::ptree const &input)> func);

  void unset_callback_raw();
  void unset_callback_json();
  void unset_callback_message();

  void run();

private:
  int handle_request(boost::fcgi::request &request);
  int handle_request_async(boost::fcgi::acceptor &acceptor, boost::fcgi::request &request);
};

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

void listener::run() {
  /// Execute the telegram service
  try {
    boost::fcgi::service service;
    boost::fcgi::acceptor acceptor(service);
    for(unsigned int i = 0; i != 2; ++i) {
      acceptor.async_accept(boost::bind(&telegram::listener::handle_request_async, this, boost::ref(acceptor), _1));
    }
    service.run();
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
  boost::property_tree::ptree tree;                                             // Create empty property tree object
  try {
    boost::iostreams::stream<boost::iostreams::array_source> stream(request.post_buffer().data(), request.post_buffer().size()); // wrap the request buffer in a stream, avoiding a copy unlike stringstream
    boost::property_tree::read_json(stream, tree);                              // Parse the json into the property tree.
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
    } catch(std::exception &e) {
      // this update doesn't include a message - no problem, carry on
    }
  }

  return boost::fcgi::commit(request, response, boost::fcgi::http::ok);         // commit the response and obtain the result
}

int listener::handle_request_async(boost::fcgi::acceptor &acceptor, boost::fcgi::request &request) {
  /// Asynchronous request handler wrapper - restarts itself after running
  std::cerr<< "LibTelegram: DEBUG: started handling an async request" << std::endl;
  int const result = handle_request(request);
  acceptor.async_accept(boost::bind(&telegram::listener::handle_request_async, this, boost::ref(acceptor), _1)); // restart the acceptor
  std::cerr<< "LibTelegram: DEBUG: finished handling an async request with result " << result << std::endl;
  return result;
}

}

#endif // TELEGRAM_LISTENER_H_INCLUDED
