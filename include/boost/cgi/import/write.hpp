//                     -- write.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_WRITE_HPP_INCLUDED
#define CGI_WRITE_HPP_INCLUDED

#include <boost/asio/write.hpp>

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  using boost::asio::write;
  using boost::asio::async_write;

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_WRITE_HPP_INCLUDED
