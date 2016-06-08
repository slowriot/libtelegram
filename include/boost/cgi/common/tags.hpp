//                   -- common/tags.hpp --
//
//            Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_COMMON_TAGS_HPP_INCLUDED__
#define CGI_COMMON_TAGS_HPP_INCLUDED__

#if _MSC_VER > 1020
#pragma once
#endif

#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  namespace tags {

    // the null tag type
    struct null {};

    // protocol types
    struct cgi {};
    struct fcgi {};
    struct scgi {};
    struct http {};

    // IoServiceProvider PoolingPolicy types
    struct single_service {};
    struct service_pool {};
    struct round_robin {};

    // Connection types
    struct stdio {};
    //struct stderr {};
    struct async_stdio {};
    struct tcp_socket {};
    // A shareable tcp_socket (ie. one that can be locked)
    struct shareable_tcp_socket {};
    struct anonymous_pipe {};
    struct fcgi_transport {};


  } // namespace tags
 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_TAGS_HPP_INCLUDED__
