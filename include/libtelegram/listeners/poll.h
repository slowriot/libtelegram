#ifndef TELEGRAM_LISTENERS_POLL_H_INCLUDED
#define TELEGRAM_LISTENERS_POLL_H_INCLUDED

#include <signal.h>
#define BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_LIB
#define BOOST_ERROR_CODE_HEADER_ONLY
#define URDL_HEADER_ONLY 1
#ifdef LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING
  #warning "SSL is disabled for outgoing messages - that is such a bad idea."
  #define URDL_DISABLE_SSL 1
#else
#endif // LIBTELEGRAM_DISABLE_SSL_NO_REALLY_I_MEAN_IT_AND_I_KNOW_WHAT_IM_DOING
#include <urdl/istream.hpp>
#include <boost/asio.hpp>
#include "base.h"
#include "libtelegram/sender.h"

void signal_handler(int s);

namespace telegram {

class sender;

namespace listener {

class poll : public base<poll> {
  telegram::sender &sender;
  static bool signal_handler_is_set;                                            // track the state of the signal handler
  std::atomic_flag keep_running;                                                // used to stop the poll externally from other threads
  static std::atomic_flag keep_running_global;                                  // used by the signal handler to terminate the poll loop, and can be modified externally

public:
  static unsigned int constexpr const poll_timeout_default = 60 * 60;           // default poll timeout in seconds
  unsigned int poll_timeout = poll_timeout_default;

  // TODO: statistics on number of times polled etc
  // TODO: statistics on requests serviced per thread etc

  inline poll(telegram::sender &sender, unsigned int poll_timeout = poll_timeout_default);

  inline void run();
  inline void stop();
  static void stop_all();

  static void set_signal_handler();
  static void unset_signal_handler();
  static bool is_signal_handler_set();
};

bool poll::signal_handler_is_set = false;
std::atomic_flag poll::keep_running_global;

inline poll::poll(telegram::sender &this_sender,
                  unsigned int this_poll_timeout)
  : sender(this_sender),
    poll_timeout(this_poll_timeout) {
  /// Default constructor
}

inline void poll::run() {
  /// Execute the telegram long-polling loop listener service
  try {
    keep_running.test_and_set();
    keep_running_global.test_and_set();
    set_signal_handler();
    boost::asio::io_service service;                                            // use asio's io_service to provide a thread pool work queue
    boost::asio::io_service::work work(service);                                // prevent the threads from running out of work
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for(unsigned int i = 0; i != num_threads; ++i) {                            // initialise the thread pool
      threads.emplace_back([&]{
        service.run();
      });
    }
    std::cout << "LibTelegram: Poll listener: Started " << threads.size() << " worker threads." << std::endl;

    int offset = 0;                                                             // keep track of the last received update offset
    while(keep_running.test_and_set() && keep_running_global.test_and_set()) {  // the poller always runs sequentially, single-threaded
      try {
        nlohmann::json tree;
        tree["offset"] = offset;
        //tree["limit"] = 100;
        tree["timeout"] = poll_timeout;
        auto reply_tree(sender.send_json("getUpdates", tree, poll_timeout));
        for(auto const &it : reply_tree["result"]) {                              // process each reply entry individually
          #ifndef NDEBUG
            std::cerr << it.dump(2) << std::endl;
          #endif // NDEBUG
          int const update_id = it["update_id"];
          if(update_id != 0) {
            offset = std::max(update_id + 1, offset);
          }
          service.post([this, subtree = it]{                                      // pass a copy of the subtree
            execute_callbacks(subtree);
          });
        }
      } catch(std::exception const &e) {                                        // catch any exceptions from the poll
        std::cerr << "LibTelegram: Exception while trying to getUpdates, cannot continue: " << e.what() << std::endl;
        stop_all();
      }
    }
    unset_signal_handler();
    service.stop();
    std::cout << "LibTelegram: Poll listener: Harvesting " << threads.size() << " worker threads...";
    for(auto &it : threads) {                                                   // close down the thread pool
      it.join();
    }
    std::cout << " Finished." << std::endl;
  } catch(boost::system::system_error const &se) {                              // this is the type of error thrown by the fcgi library.
    std::cerr << "LibTelegram: System error: " << se.what() << std::endl;
  } catch(std::exception const &e) {                                            // catch any other exceptions
    std::cerr << "LibTelegram: Exception: " << e.what() << std::endl;
  } catch(...) {
    std::cerr<< "LibTelegram: Uncaught exception!" << std::endl;
  }
}

inline void poll::stop() {
  /// Request that the listener currently running stops gracefully, at the end of its current long poll.
  /// It is only meaningful to call this from another thread while poll::run() is running.
  keep_running.clear();
}
void poll::stop_all() {
  /// Request that the listener currently running stops gracefully, at the end of its current long poll.
  /// It is only meaningful to call this from another thread or signal handler while poll::run() is running.
  /// Note that this will stop ALL polling listeners globally.
  keep_running_global.clear();
}

void poll::set_signal_handler() {
  /// Set a signal handler to catch ctrl-c in the console and close gracefully
  #ifndef TELEGRAM_NO_SIGNAL_HANDLER
    struct sigaction signal;
    signal.sa_handler = signal_handler;
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = 0;
    sigaction(SIGINT, &signal, NULL);
    signal_handler_is_set = true;
  #endif // TELEGRAM_NO_SIGNAL_HANDLER
}
void poll::unset_signal_handler() {
  /// Unset any signal handler we set earlier
  #ifndef TELEGRAM_NO_SIGNAL_HANDLER
    struct sigaction signal;
    signal.sa_handler = SIG_DFL;
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = 0;
    sigaction(SIGINT, &signal, NULL);
    signal_handler_is_set = false;
  #endif // TELEGRAM_NO_SIGNAL_HANDLER
}
bool poll::is_signal_handler_set() {
  /// Report whether our signal handler is currently set
  return signal_handler_is_set;
}

}
}

void signal_handler(int s) {
  /// Signal handler for ctrl-c events
  std::cout << std::endl;
  std::cout << "LibTelegram: Poll listener: Caught signal " << s << ", terminating after this poll..." << std::endl;
  telegram::listener::poll::stop_all();
  telegram::listener::poll::unset_signal_handler();                             // unset the signal handler, so a second ctrl-c will exit immediately
}

#endif // TELEGRAM_LISTENERS_POLL_H_INCLUDED
