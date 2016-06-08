//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_CGI_SCGI_REQUEST_IMPL_HPP_INCLUDED_
#define BOOST_CGI_SCGI_REQUEST_IMPL_HPP_INCLUDED_

#include <string>
#include <vector>

BOOST_CGI_NAMESPACE_BEGIN
   namespace scgi {

struct header_impl
{
  std::string name;
  std::string value;
};

/// A request received from a client.
struct request_impl
{
  /// The headers included with the request.
  std::vector<header_impl> headers;

  /// The optional content sent with the request.
  std::string content;

  /// The length of the headers.
  std::size_t headers_length_;
  std::size_t content_length_;
};

 } // namespace scgi
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_SCGI_REQUEST_IMPL_HPP_INCLUDED_
