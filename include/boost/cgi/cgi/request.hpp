//                 -- cgi/request.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_REQUEST_HPP_INCLUDED__
#define CGI_CGI_REQUEST_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include "boost/cgi/cgi/traits.hpp"
#include "boost/cgi/cgi/service.hpp"
#include "boost/cgi/cgi/request_service.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  typedef
    common::basic_request<
        common::tags::cgi
    >
  cgi_request;
  
  typedef cgi_request request;

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/basic_request.hpp"

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_CGI_REQUEST_HPP_INCLUDED__
