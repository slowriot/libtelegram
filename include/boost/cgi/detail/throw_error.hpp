//                  -- throw_error.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_THROW_ERROR_HPP_INCLUDED__
#define CGI_THROW_ERROR_HPP_INCLUDED__

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/throw_exception.hpp>
/////////////////////////////////////////////////
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace detail {

   inline void throw_error(const boost::system::error_code& ec)
   {
     if(ec)
     {
       boost::system::system_error err(ec);
       //boost::throw_exception(err);
       throw err;
     }
   }

 } // namespace detail
BOOST_CGI_NAMESPACE_END

#endif // CGI_THROW_ERROR_HPP_INCLUDED__
