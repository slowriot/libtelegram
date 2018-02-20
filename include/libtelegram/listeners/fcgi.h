#ifndef TELEGRAM_LISTENERS_FCGI_H_INCLUDED
#define TELEGRAM_LISTENERS_FCGI_H_INCLUDED

#include "libtelegram/config.h"

#include <boost/cgi/fcgi.hpp>
#include <boost/iostreams/stream.hpp>
#include "base.h"

namespace telegram::listener {

class fcgi : public base<fcgi> {
public:
  void run();

private:
  inline int handle_request(boost::fcgi::request &request);
  inline int handle_request_async(boost::fcgi::acceptor &acceptor, boost::fcgi::request &request);
};

void fcgi::run() {
  /// Execute the telegram fcgi listener service
  try {
    boost::fcgi::service service;
    boost::fcgi::acceptor acceptor(service);
    for(unsigned int i = 0; i != 2; ++i) {
      acceptor.async_accept(boost::bind(&telegram::listener::fcgi::handle_request_async, this, boost::ref(acceptor), _1));
    }
    if(num_threads == 1) {
      service.run();                                                            // single-threaded operation runs the service on the main thread
    } else {
      std::vector<std::thread> threads;
      for(unsigned int i = 0; i != num_threads; ++i) {                          // run however many threads we've requested
        threads.emplace_back([&service]{
          service.run();
        });
        //thread_statistics stats;
        //stats.thread_num = i;
        //thread_stats.insert(std::make_pair(threads.back().get_id(), stats));
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

inline int fcgi::handle_request(boost::fcgi::request &request) {
  /// Request handler - parse what we received and dispatch callbacks appropriately
  #ifndef NDEBUG
    std::cerr<< "LibTelegram: FastCGI: DEBUG: started handling a request" << std::endl;
  #endif // NDEBUG
  boost::system::error_code error;
  request.load(boost::fcgi::parse_form, error);                                 // see boost/cgi/common/parse_options.hpp
  boost::fcgi::response response;
  response << boost::fcgi::content_type("text/html");                           // set a content type for the normal content output
  if(error) {
    std::cerr << "LibTelegram: Request parsing error: " << error.message() << std::endl;
    response << "Request parsing error: " << error.message();
    return boost::fcgi::commit(request, response, boost::fcgi::http::internal_server_error);
  }
  if(request.method() != "POST") {                                              // is this a post request?
    response << "Bad request.";                                                 // this probably didn't come from Telegram, so don't give away any additional information
    return boost::fcgi::commit(request, response, boost::fcgi::http::bad_request);
  }

  // deal with the data
  nlohmann::json tree;                                                          // create empty json object
  try {
    boost::iostreams::stream<boost::iostreams::array_source> stream(request.post_buffer().data(), request.post_buffer().size()); // wrap the request buffer in a stream, avoiding a copy unlike stringstream
    stream >> tree;
  } catch(std::exception &e) {
    std::cerr << "LibTelegram: Received unparseable JSON: " << e.what() << std::endl;
    response << "Received unparseable JSON";                                    // if we got this far, complain what exactly fucked up
    return boost::fcgi::commit(request, response, boost::fcgi::http::bad_request);
  }
  response << "OK";                                                             // the json must be fine, so generate a minimal reply
  auto result = boost::fcgi::commit(request, response, boost::fcgi::http::ok);  // commit the response and obtain the result, so we can let Telegram hang up and it won't resend even if callbacks crash

  if(callback_raw) {
    callback_raw(request.post_buffer());                                        // if the raw callback is set, send the whole buffer straight there
  }
  execute_callbacks(tree);                                                      // call the remaining callbacks on this tree

  return result;                                                                // return the cached result
}

inline int fcgi::handle_request_async(boost::fcgi::acceptor &acceptor, boost::fcgi::request &request) {
  /// Asynchronous request handler wrapper - restarts itself after running
  int const result = handle_request(request);
  acceptor.async_accept(boost::bind(&telegram::listener::fcgi::handle_request_async, this, boost::ref(acceptor), _1)); // restart the acceptor
  return result;
}

}

#endif // TELEGRAM_LISTENERS_FCGI_H_INCLUDED
