//                     -- error.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// Abstract:
// ---------
//
// The errors for everything are defined in here. ie. FastCGI,
// CGI and SCGI errors.
//
// **FIXME** There should likely be a common set of errors and
//           then protocol-specific errors. So far, everything
//           is an fcgi_error, which clearly isn't right.
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ERROR_HPP_INCLUDED__
#define CGI_ERROR_HPP_INCLUDED__

#include <string>
#include <boost/system/error_code.hpp>
#include "boost/cgi/common/error.hpp"
#include "boost/cgi/fcgi/error.hpp"

BOOST_CGI_NAMESPACE_BEGIN
  namespace error {

    using namespace ::BOOST_CGI_NAMESPACE::common::error;

  } // namespace error
BOOST_CGI_NAMESPACE_END

#endif // CGI_ERROR_HPP_INCLUDED__

