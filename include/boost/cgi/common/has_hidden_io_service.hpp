//                -- common/is_async.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_COMMON_HAS_HIDDEN_IO_SERVICE_HPP_INCLUDED_20091203_
#define CGI_COMMON_HAS_HIDDEN_IO_SERVICE_HPP_INCLUDED_20091203_

#include <boost/mpl/bool.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   template<typename Protocol>
   struct has_hidden_io_service
     : boost::mpl::bool_<false>
   {
   };

   template<>
   struct has_hidden_io_service<tags::cgi>
    : boost::mpl::bool_<true>
   {
   };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_COMMON_HAS_HIDDEN_IO_SERVICE_HPP_INCLUDED_20091203_

