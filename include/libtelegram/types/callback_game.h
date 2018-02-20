#ifndef TELEGRAM_TYPES_CALLBACK_GAME_H_INCLUDED
#define TELEGRAM_TYPES_CALLBACK_GAME_H_INCLUDED

#include "libtelegram/config.h"

#include "message.h"

namespace telegram::types {

struct callback_game {
  /// See https://core.telegram.org/bots/api#callbackgame
  // At the time of writing, currently a placeholder holding no information.

  inline void get(nlohmann::json &tree) const;
};

inline void callback_game::get(nlohmann::json &tree [[maybe_unused]]) const {
  /// Get this keyboard button into a json object (presuming the target is a key in a keyboard)
  // noop
}

}

#endif // TELEGRAM_TYPES_CALLBACK_GAME_H_INCLUDED
