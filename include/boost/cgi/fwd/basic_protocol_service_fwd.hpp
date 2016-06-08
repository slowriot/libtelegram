//          -- basic_protocol_service_fwd.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_PROTOCOL_SERVICE_FWD_HPP_INCLUDED__
#define CGI_BASIC_PROTOCOL_SERVICE_FWD_HPP_INCLUDED__

#if _MSC_VER > 1020
#pragma once
#endif

#include "boost/cgi/fwd/io_service_provider_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  template<
    typename Protocol,
    typename IoServiceProvider = common::io_service_provider<>
  >
  class basic_protocol_service;

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_BASIC_PROTOCOL_SERVICE_FWD_HPP_INCLUDED__

