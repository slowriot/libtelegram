//                  -- cgi_service.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_SERVICE_HPP_INCLUDED__
#define CGI_CGI_SERVICE_HPP_INCLUDED__

#include "boost/cgi/cgi/traits.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  typedef common::basic_protocol_service<common::tags::cgi> cgi_service;
  /// typedef for typical usage
  namespace cgi {
    typedef cgi_service service;
  }
  typedef cgi_service service;

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/common/basic_protocol_service.hpp"

#endif // CGI_CGI_SERVICE_HPP_INCLUDED__

