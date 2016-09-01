#ifndef TELEGRAM_LISTENERS_POLL_H_INCLUDED
#define TELEGRAM_LISTENERS_POLL_H_INCLUDED

#include <urdl/istream.hpp>
#include "base.h"
#include "libtelegram/sender.h"

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
    // this always runs sequentially, single-threaded
    int offset = 0;
    for(;;) {
      nlohmann::json tree;
      tree["offset"] = offset;
      //tree["limit"] = 100;
      tree["timeout"] = poll_timeout;
      auto reply_tree(sender.send_json("getUpdates", tree, poll_timeout));

      for(auto const &it : reply_tree["result"]) {
        std::cerr << it.dump(2) << std::endl;
        int const update_id = it["update_id"];
        if(update_id != 0) {
          offset = std::max(update_id + 1, offset);
        }
        execute_callbacks(it);                                                  // process the callbacks for each reply
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

}
}

#endif // TELEGRAM_LISTENERS_POLL_H_INCLUDED
