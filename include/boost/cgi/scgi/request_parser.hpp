//             -- scgi/request_parser.hpp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_PARSER_HPP_INCLUDED__
#define CGI_SCGI_REQUEST_PARSER_HPP_INCLUDED__

#include <string>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include "boost/cgi/detail/coroutine.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace scgi {

struct request_impl;

/// Parser for incoming requests.
class request_parser : coroutine
{
public:
  /// Parse some data. The tribool return value is true when a complete request
  /// has been parsed, false if the data is invalid, indeterminate when more
  /// data is required. The InputIterator return value indicates how much of the
  /// input has been consumed.
  template <typename InputIterator>
  boost::tuple<boost::tribool, InputIterator> parse(request_impl& req,
      InputIterator begin, InputIterator end)
  {
    while (begin != end)
    {
      std::cerr<< *begin;
      boost::tribool result = consume(req, *begin++);
      if (result || !result)
        return boost::make_tuple(result, begin);
    }
    boost::tribool result = boost::indeterminate;
    return boost::make_tuple(result, begin);
  }

private:
  /// The name of the content length header.
  static std::string content_length_name_;

  /// Whether writing the header name at the moment.
  bool writing_header_name_;

  /// Length of the headers. Always required.
  std::size_t headers_length_;

  /// Content length as decoded from headers. Defaults to 0.
  std::size_t content_length_;

  /// Handle the next character of input.
  boost::tribool consume(request_impl& req, char input);
};

 } // namespace scgi
BOOST_CGI_NAMESPACE_END

#endif // CGI_SCGI_REQUEST_PARSER_HPP_INCLUDED__
