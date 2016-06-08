//                -- scgi/acceptor.hpp --
//
//           Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_ACCEPTOR_HPP_INCLUDED__
#define CGI_SCGI_ACCEPTOR_HPP_INCLUDED__

#include "boost/cgi/scgi/traits.hpp"
#include "boost/cgi/basic_request_acceptor.hpp"
#include "boost/cgi/scgi/request_acceptor_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace scgi {

   /// Typedef for common usage (SCGI)
   typedef
     ::BOOST_CGI_NAMESPACE::common::basic_request_acceptor<
        ::BOOST_CGI_NAMESPACE::common::tags::scgi
       >
   acceptor;

 } // namespace scgi
BOOST_CGI_NAMESPACE_END

namespace boost { namespace scgi {
  using ::BOOST_CGI_NAMESPACE::scgi::acceptor;
} }

#endif // CGI_SCGI_ACCEPTOR_HPP_INCLUDED__
