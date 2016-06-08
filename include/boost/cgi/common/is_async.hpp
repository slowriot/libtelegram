//                -- common/is_async.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_COMMON_IS_ASYNC_HPP_INCLUDED__
#define CGI_COMMON_IS_ASYNC_HPP_INCLUDED__

#include <boost/mpl/bool.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   template<typename Protocol>
   struct is_async
     : boost::mpl::bool_<true>
   {
     //typedef boost::mpl::bool_<true>::type::value value;
   };

   template<>
   struct is_async<tags::cgi>
    : boost::mpl::bool_<false>
   {
     //typedef boost::mpl::bool_<false>::type::value value;
   };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_COMMON_IS_ASYNC_HPP_INCLUDED__

