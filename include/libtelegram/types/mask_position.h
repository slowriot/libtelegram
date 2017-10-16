#ifndef TELEGRAM_TYPES_MASK_POSITION_H_INCLUDED
#define TELEGRAM_TYPES_MASK_POSITION_H_INCLUDED

#include "helpers/optional.h"

namespace telegram::types {

struct mask_position {
  /// See https://core.telegram.org/bots/api#maskposition
  enum class point_types {
    FOREHEAD,
    EYES,
    MOUTH,
    CHIN,
    UNKNOWN
  } point = point_types::UNKNOWN;                                               // The part of the face relative to which the mask should be placed. One of “forehead”, “eyes”, “mouth”, or “chin”.
  float x_shift = 0.0f;                                                         // Shift by X-axis measured in widths of the mask scaled to the face size, from left to right. For example, choosing -1.0 will place mask just to the left of the default mask position.
  float y_shift = 0.0f;                                                         // Shift by Y-axis measured in heights of the mask scaled to the face size, from top to bottom. For example, 1.0 will place the mask just below the default mask position.
  float scale   = 0.0f;                                                         // Mask scaling coefficient. For example, 2.0 means double size.

  static mask_position const from_json(nlohmann::json const &tree);
  static mask_position const from_json(nlohmann::json const &tree, std::string const &path);
};

inline mask_position const mask_position::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  std::string const &point_str{tree.at("point").get<std::string>()};
  point_types this_point;
  if(point_str == "forehead") {
    this_point = point_types::FOREHEAD;
  } else if(point_str == "eyes") {
    this_point = point_types::EYES;
  } else if(point_str == "mouth") {
    this_point = point_types::MOUTH;
  } else if(point_str == "chin") {
    this_point = point_types::CHIN;
  } else {
    this_point = point_types::UNKNOWN;
  }
  return mask_position{this_point,
                       tree.at("x_shift"),
                       tree.at("y_shift"),
                       tree.at("scale")};
}
inline mask_position const mask_position::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

}

#endif // TELEGRAM_TYPES_MASK_POSITION_H_INCLUDED
