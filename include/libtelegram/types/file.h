#ifndef TELEGRAM_TYPES_FILE_H_INCLUDED
#define TELEGRAM_TYPES_FILE_H_INCLUDED

#include "libtelegram/config.h"

#include "urdl/url.hpp"
#include "urdl/istream.hpp"
#include "urdl/http.hpp"
#include "helpers/optional.h"

namespace telegram::types {

struct file {
  /// See https://core.telegram.org/bots/api#file
  std::string file_id;                                                          // Unique identifier for this file
  std::optional<int_fast32_t> file_size;                                        // Optional. File size, if known (in bytes)
  std::optional<std::string> file_path;                                         // Optional. File path. Use https://api.telegram.org/file/bot<token>/<file_path> to get the file.

  static file const from_json(nlohmann::json const &tree);
  static file const from_json(nlohmann::json const &tree, std::string const &path);

  inline urdl::url const get_url(std::string const &token) const;

  inline void download_to_stream(std::string const &token, urdl::istream &target_stream) const;

  inline std::vector<char> download(std::string const &token) const;
};

inline file const file::from_json(nlohmann::json const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws std::domain_error
  return file{tree.at("file_id"),
              helpers::make_optional_from_json<int32_t>(tree, "file_size"),
              helpers::make_optional_from_json<std::string>(tree, "file_path")};
}
inline file const file::from_json(nlohmann::json const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws std::domain_error
  return from_json(tree.at(path));
}

inline urdl::url const file::get_url(std::string const &token) const {
  /// Return the URL of this file on Telegram's servers
  /// NOTE: You don't want to share this URL - it contains your bot's token!
  if(file_path) {
    return urdl::url("https://api.telegram.org/file/bot" + token + "/" + *file_path);
  } else {
    return urdl::url();
  }
}

inline void file::download_to_stream(std::string const &token, urdl::istream &stream) const {
  /// Download this file to the target output stream
  stream.set_option(urdl::http::user_agent("LibTelegram File Downloader"));
  stream.open_timeout(60000);
  stream.read_timeout(600000);
  auto const &url(get_url(token));
  stream.open(url);
  if(!stream) {
    std::cerr << "LibTelegram: File: Unable to open URL " << url.to_string() << ": " << stream.error().message() << std::endl;
  }
}

inline std::vector<char> file::download(std::string const &token) const {
  /// Download this file to the target output stream
  urdl::istream stream;
  download_to_stream(token, stream);
  std::vector<char> out;
  if(file_size) {
    out.reserve(*file_size);                                                    // attempt to reserve the right space but only if it's known
  }
  //stream.read(&out[0], file_size);
  //stream.read(out.data(), file_size);
  out.assign(std::istreambuf_iterator<char>(stream),
             std::istreambuf_iterator<char>());
  out.shrink_to_fit();
  return out;
}

}

#endif // TELEGRAM_TYPES_FILE_H_INCLUDED
