//                -- fcgi/acceptor.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_ACCEPTOR_HPP_INCLUDED__
#define CGI_FCGI_ACCEPTOR_HPP_INCLUDED__

#include "boost/cgi/fcgi/traits.hpp"
#include "boost/cgi/basic_request_acceptor.hpp"
#include "boost/cgi/fcgi/request_acceptor_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace fcgi {

   /// Typedef for common usage (FCGI)
   typedef
     ::BOOST_CGI_NAMESPACE::common::basic_request_acceptor<
        ::BOOST_CGI_NAMESPACE::common::tags::fcgi
       >
   acceptor;

 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

namespace boost { namespace fcgi {
  using ::BOOST_CGI_NAMESPACE::fcgi::acceptor;
} }

#endif // CGI_FCGI_ACCEPTOR_HPP_INCLUDED__
