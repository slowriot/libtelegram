//                -- fcgi/request_fwd.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_REQUEST_FWD_HPP_INCLUDED__
#define CGI_FCGI_REQUEST_FWD_HPP_INCLUDED__

#include "boost/cgi/basic_request_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN
  namespace fcgi {

    typedef service service_t;
    template<typename Protocol>
    class fcgi_request_service;

    typedef basic_request<fcgi_request_service<common::tags::fcgi>, service_t> request;

  } // namespace fcgi
BOOST_CGI_NAMESPACE_END

#endif // CGI_FCGI_REQUEST_FWD_HPP_INCLUDED__

