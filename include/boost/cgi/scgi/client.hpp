//                 -- scgi/client.hpp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_CLIENT_HPP_INCLUDED__
#define CGI_SCGI_CLIENT_HPP_INCLUDED__

#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/scgi/traits.hpp"
#include "boost/cgi/connections/tcp_socket.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/error.hpp"
#include "boost/cgi/import/read.hpp"
#include "boost/cgi/import/buffer.hpp"
#include "boost/cgi/import/io_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN

namespace scgi {
    typedef
      common::basic_client<
        ::BOOST_CGI_NAMESPACE::common::tags::scgi
      >
    client;
} // namespace scgi

BOOST_CGI_NAMESPACE_END

#endif // CGI_SCGI_CLIENT_HPP_INCLUDED__
