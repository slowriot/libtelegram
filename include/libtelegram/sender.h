#ifndef TELEGRAM_SENDER_H_INCLUDED
#define TELEGRAM_SENDER_H_INCLUDED

#include <string>

namespace telegram {

class sender {
  std::string token;

public:
  sender(std::string const &token);
};

sender::sender(std::string const &this_token)
  : token(this_token) {
  /// Construct a sender with the given token
}

}

#endif // TELEGRAM_SENDER_H_INCLUDED
