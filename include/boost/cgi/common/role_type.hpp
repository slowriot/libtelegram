//                   -- role_type.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ROLE_TYPE_HPP_INCLUDED__
#define CGI_ROLE_TYPE_HPP_INCLUDED__

#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {
  namespace role {

   struct responder {};
   struct authorizer {};
   struct filter {};

 } // namespace role
 
  enum role_type
  { none
  , responder
  , authorizer
  , filter
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_ROLE_TYPE_HPP_INCLUDED__
