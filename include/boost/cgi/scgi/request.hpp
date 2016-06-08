//                 -- scgi/request.hpp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_HPP_INCLUDED__
#define CGI_SCGI_REQUEST_HPP_INCLUDED__

#include "boost/cgi/scgi/traits.hpp"
#include "boost/cgi/scgi/service.hpp"
#include "boost/cgi/scgi/request_service.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace scgi {
   
   // typedef for typical usage (SCGI)
   typedef
     common::basic_request<
          common::tags::scgi
     >
   request;

 } // namespace scgi
BOOST_CGI_NAMESPACE_END

namespace boost { namespace scgi {
  using ::BOOST_CGI_NAMESPACE::scgi::request;
} }

#include "boost/cgi/basic_request.hpp"

#endif // CGI_SCGI_REQUEST_HPP_INCLUDED__
