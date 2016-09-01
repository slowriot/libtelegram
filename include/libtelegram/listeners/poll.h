#ifndef TELEGRAM_LISTENERS_POLL_H_INCLUDED
#define TELEGRAM_LISTENERS_POLL_H_INCLUDED

#include <signal.h>
#include <urdl/istream.hpp>
#include "base.h"
#include "libtelegram/sender.h"

bool keep_running = true;                                                       // used by the signal handler to terminate the poll loop
void signal_handler(int s);

namespace telegram {

class sender;

namespace listener {

class poll : public base<poll> {
  telegram::sender &sender;
public:
  static unsigned int constexpr const poll_timeout_default = 60 * 60;           // default poll timeout in seconds
  unsigned int poll_timeout = poll_timeout_default;

  // TODO: statistics on number of times polled etc

  poll(telegram::sender &sender, unsigned int poll_timeout = poll_timeout_default);

  void run();
};

poll::poll(telegram::sender &this_sender,
           unsigned int this_poll_timeout)
  : sender(this_sender),
    poll_timeout(this_poll_timeout) {
  /// Default constructor
}

void poll::run() {
  /// Execute the telegram long-polling loop listener service
  try {
    {
      // set a signal handler to catch ctrl-c in the console and close gracefully
      struct sigaction signal;
      signal.sa_handler = signal_handler;
      sigemptyset(&signal.sa_mask);
      signal.sa_flags = 0;
      sigaction(SIGINT, &signal, NULL);
    }
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
    while(keep_running) {                                                       // the poller always runs sequentially, single-threaded
      nlohmann::json tree;
      tree["offset"] = offset;
      //tree["limit"] = 100;
      tree["timeout"] = poll_timeout;
      auto reply_tree(sender.send_json("getUpdates", tree, poll_timeout));
      for(auto const &it : reply_tree["result"]) {                              // process each reply entry individually
        std::cerr << it.dump(2) << std::endl;
        int const update_id = it["update_id"];
        if(update_id != 0) {
          offset = std::max(update_id + 1, offset);
        }
        service.post([this, subtree = it]{                                      // pass a copy of the subtree
          execute_callbacks(subtree);
        });
      }
    }
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

}
}

void signal_handler(int s) {
  /// Signal handler for ctrl-c events
  std::cout << std::endl;
  std::cout << "LibTelegram: Poll listener: Caught signal " << s << ", terminating after this poll..." << std::endl;
  keep_running = false;
  {
    // unset the signal handler, so a second ctrl-c will exit immediately
    struct sigaction signal;
    signal.sa_handler = SIG_DFL;
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = 0;
    sigaction(SIGINT, &signal, NULL);
  }
}

#endif // TELEGRAM_LISTENERS_POLL_H_INCLUDED
