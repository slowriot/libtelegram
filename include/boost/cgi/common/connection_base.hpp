//                -- connection_base.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CONNECTION_BASE_HPP_INCLUDED__
#define CGI_CONNECTION_BASE_HPP_INCLUDED__

#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  /// Abstract Base Class for all connection types
  class connection_base
  {
  public:
    virtual ~connection_base() {}
  };

BOOST_CGI_NAMESPACE_END

#endif // CGI_CONNECTION_BASE_HPP_INCLUDED__
