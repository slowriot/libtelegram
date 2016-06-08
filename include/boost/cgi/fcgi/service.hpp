//                 -- fcgi/service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_SERVICE_HPP_INCLUDED__
#define CGI_FCGI_SERVICE_HPP_INCLUDED__

#include "boost/cgi/fcgi/traits.hpp"
#include "boost/cgi/common/basic_protocol_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace fcgi {
 
   // typedef for standard fcgi::service (a model of ProtocolService)
   typedef
   common::basic_protocol_service<
       common::tags::fcgi
     >
   service;
   
 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

namespace boost { namespace fcgi {
  using ::BOOST_CGI_NAMESPACE::fcgi::service;
} }

#endif // CGI_FCGI_SERVICE_HPP_INCLUDED__
