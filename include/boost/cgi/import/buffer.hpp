//                    -- buffer.hpp --
//
//            Copyright (c) Darren Garvey 2007
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BUFFER_HPP_INCLUDED__
#define CGI_BUFFER_HPP_INCLUDED__

#include "boost/cgi/config.hpp"
#include <boost/asio/buffer.hpp>

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// Import the Boost.Asio overloads.
  using boost::asio::buffer;

 } // namespace common

  using common::buffer;

BOOST_CGI_NAMESPACE_END

#endif // CGI_HPP_INCLUDED__
