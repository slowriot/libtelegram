//                -- protocol_traits.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_PROTOCOL_TRAITS_HPP_INCLUDED_
#define BOOST_CGI_PROTOCOL_TRAITS_HPP_INCLUDED_

#include <boost/asio/ip/tcp.hpp>
#include <boost/none.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/config.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/fcgi/specification.hpp"
#include "boost/cgi/fwd/basic_client_fwd.hpp"
#include "boost/cgi/fwd/basic_connection_fwd.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/fwd/form_parser_fwd.hpp"
#ifdef BOOST_CGI_ENABLE_SESSIONS
#  include "boost/cgi/fwd/sessions_fwd.hpp"
#endif // BOOST_CGI_ENABLE_SESSIONS

BOOST_CGI_NAMESPACE_BEGIN

 namespace common {
 
    enum session_opts
    {
      path_session,
      domain_session
    };

    template<typename Protocol>
    struct protocol_traits
    {
    };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_PROTOCOL_TRAITS_HPP_INCLUDED_
