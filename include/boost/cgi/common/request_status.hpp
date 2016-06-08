//              -- common/request_status.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_COMMON_REQUEST_STATUS_HPP_INCLUDED__
#define CGI_COMMON_REQUEST_STATUS_HPP_INCLUDED__
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   enum request_status
     { null           = 0
     , unloaded       = null
     , activated      = 1
     , accepted       = activated
     , begin_request_found = 2
     , env_read       = 4
     , get_read       = 8
     , post_read      = 16
     , cookies_read   = 32
     , loaded         = 64
     , ok             = loaded
     , aborted
     , closed
     };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_COMMON_REQUEST_STATUS_HPP_INCLUDED__

