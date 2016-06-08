//                   -- cgi/cgi.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_HPP_INCLUDED__
#define CGI_CGI_HPP_INCLUDED__

#include "boost/cgi/config.hpp"

// Include all cgi-related headers only.
#include "boost/cgi/cgi/traits.hpp"
#include "boost/cgi/cgi/request.hpp"
#include "boost/cgi/cgi/client.hpp"
#include "boost/cgi/cgi/service.hpp"

// Include headers common to all protocols
#include "boost/cgi/detail/common_headers.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  typedef cgi_request request;
  using namespace common;
  
BOOST_CGI_NAMESPACE_END

#endif // CGI_CGI_HPP_INCLUDED__
