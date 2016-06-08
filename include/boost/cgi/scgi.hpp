//                   -- cgi/scgi.hpp --
//
//           Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_HPP_INCLUDED__
#define CGI_SCGI_HPP_INCLUDED__

#include "boost/cgi/config.hpp"

// Include all SCGI-related headers.
#include "boost/cgi/scgi/traits.hpp"
#include "boost/cgi/scgi/request.hpp"
#include "boost/cgi/scgi/client.hpp"
#include "boost/cgi/scgi/service.hpp"
#include "boost/cgi/scgi/acceptor.hpp"

// Include headers common to all protocols
#include "boost/cgi/detail/common_headers.hpp"

BOOST_CGI_NAMESPACE_BEGIN
  namespace scgi {

  using namespace common;
  
 } // namespace scgi
BOOST_CGI_NAMESPACE_END

namespace boost { namespace scgi {

  using namespace BOOST_CGI_NAMESPACE::scgi;

} } // namespace boost::scgi

#endif // CGI_SCGI_HPP_INCLUDED__
