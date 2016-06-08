//                  -- cgi/client.hpp --
//
//          Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CLIENT_HPP_INCLUDED__
#define CGI_CLIENT_HPP_INCLUDED__

#include "boost/cgi/cgi/traits.hpp"
#include "boost/cgi/fwd/basic_client_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  typedef common::basic_client<common::tags::cgi> cgi_client;
  /// typedef for typical usage
  typedef cgi_client client;

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/basic_client.hpp"

#endif // CGI_CLIENT_HPP_INCLUDED__
