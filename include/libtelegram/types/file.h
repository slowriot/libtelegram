#ifndef TELEGRAM_TYPES_FILE_H_INCLUDED
#define TELEGRAM_TYPES_FILE_H_INCLUDED

#define URDL_HEADER_ONLY 1
#include "urdl/url.hpp"
#include "urdl/istream.hpp"
#include "urdl/http.hpp"

namespace telegram {
namespace types {

struct file {
  /// See https://core.telegram.org/bots/api#file
  std::string file_id;                                                          // Unique identifier for this file
  int_fast32_t file_size = 0;                                                   // Optional. File size, if known
  std::string file_path;                                                        // Optional. File path. Use https://api.telegram.org/file/bot<token>/<file_path> to get the file.

  static file const from_ptree(boost::property_tree::ptree const &tree);
  static file const from_ptree(boost::property_tree::ptree const &tree, std::string const &path);

  urdl::url const get_url(std::string const &token) const;

  void download_to_stream(std::string const &token, urdl::istream &target_stream) const;

  std::vector<char> download(std::string const &token) const;
};

file const file::from_ptree(boost::property_tree::ptree const &tree) {
  /// Factory to generate a struct of this type from the correct property tree
  /// If any non-optional elements are missing from the tree, throws boost::property_tree::ptree_bad_path
  return file{tree.get("file_id", ""),
              tree.get("file_size", 0),
              tree.get("file_path", "")};
}
file const file::from_ptree(boost::property_tree::ptree const &tree, std::string const &path) {
  /// Helper to generate a struct of this type from a path within a tree
  /// If there is no such child, throws boost::property_tree::ptree_bad_path
  return from_ptree(tree.get_child(path));
}

urdl::url const file::get_url(std::string const &token) const {
  /// Return the URL of this file on Telegram's servers
  /// NOTE: You don't want to share this URL - it contains your bot's token!
  return urdl::url("https://api.telegram.org/file/bot" + token + "/" + file_path);
}

void file::download_to_stream(std::string const &token, urdl::istream &stream) const {
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

std::vector<char> file::download(std::string const &token) const {
  /// Download this file to the target output stream
  urdl::istream stream;
  download_to_stream(token, stream);
  std::vector<char> out;
  out.reserve(file_size);
  //stream.read(&out[0], file_size);
  //stream.read(out.data(), file_size);
  out.assign(std::istreambuf_iterator<char>(stream),
             std::istreambuf_iterator<char>());
  return out;
}

}
}

#endif // TELEGRAM_TYPES_FILE_H_INCLUDED
