//                    -- fcgi.hpp --
//
//         Copyright (c) Darren Garvey 2007-2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_HPP_INCLUDED__
#define CGI_FCGI_HPP_INCLUDED__

// Include all FastCGI-related headers
#include "boost/cgi/config.hpp"
#include "boost/cgi/fcgi/traits.hpp"
#include "boost/cgi/fcgi/acceptor.hpp"
#include "boost/cgi/fcgi/client.hpp"
#include "boost/cgi/fcgi/request.hpp"
#include "boost/cgi/fcgi/service.hpp"

// Include common headers.
#include "boost/cgi/detail/common_headers.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace fcgi {
   using namespace ::BOOST_CGI_NAMESPACE::common; // import common elements.
 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

/// Dump fcgi stuff into the boost namespace
namespace boost {
 namespace fcgi {
   using namespace ::BOOST_CGI_NAMESPACE::fcgi;
   using namespace ::BOOST_CGI_NAMESPACE::common; // import common elements.
 } // namespace fcgi
} // namespace boost

#endif // CGI_FCGI_HPP_INCLUDED__
