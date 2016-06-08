#ifndef TELEGRAM_LISTENER_H_INCLUDED
#define TELEGRAM_LISTENER_H_INCLUDED

#include <functional>
#include <iostream>
#define BOOST_CGI_NO_BOOST_FILESYSTEM
#include <boost/cgi/fcgi.hpp>

namespace telegram {

class listener {
  std::function<void(std::string const &input)> callback = [](std::string const &input __attribute__((__unused__))){};

public:
  void register_callback(std::function<void(std::string const &input)> func);

  void run();

private:
  int handle_request(boost::fcgi::request &request);
  int handle_request_async(boost::fcgi::acceptor &acceptor, boost::fcgi::request &request);
};

void listener::register_callback(std::function<void(std::string const &input)> func) {
  callback = func;
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

  // generate a reply
  response << boost::fcgi::content_type("text/html");                           // set a content type for the normal content output
  response << "<pre>";
  response << "hello world from program " << request.script_name();
  response << "</pre>";

  int const result = boost::fcgi::commit(request, response, 0);                 // commit the response and obtain the result
  return result;
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
