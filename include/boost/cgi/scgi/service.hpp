//                 -- scgi/service.hpp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_SERVICE_HPP_INCLUDED__
#define CGI_SCGI_SERVICE_HPP_INCLUDED__

#include "boost/cgi/scgi/traits.hpp"
#include "boost/cgi/common/basic_protocol_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace scgi {
 
   // typedef for standard scgi::service (a model of ProtocolService)
   typedef
   common::basic_protocol_service<
       common::tags::scgi
     >
   service;
   
 } // namespace scgi
BOOST_CGI_NAMESPACE_END

namespace boost { namespace scgi {
  using ::BOOST_CGI_NAMESPACE::scgi::service;
} }

#endif // CGI_SCGI_SERVICE_HPP_INCLUDED__
