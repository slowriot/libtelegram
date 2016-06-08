//                 -- scgi/traits.hpp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_SCGI_TRAITS_HPP_INCLUDED_
#define BOOST_CGI_SCGI_TRAITS_HPP_INCLUDED_

#include <boost/asio/ip/tcp.hpp>
#include <boost/none.hpp>
#include <boost/config.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/fwd/basic_client_fwd.hpp"
#include "boost/cgi/fwd/basic_connection_fwd.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/fwd/form_parser_fwd.hpp"
#ifdef BOOST_CGI_ENABLE_SESSIONS
#  include "boost/cgi/fwd/sessions_fwd.hpp"
#endif // BOOST_CGI_ENABLE_SESSIONS

BOOST_CGI_NAMESPACE_BEGIN

  namespace scgi
  {
  class scgi_service_impl;
  template<typename Protocol>
  class scgi_request_service;
  class scgi_acceptor_service;
  }

  // Forward declarations

  template<typename Protocol>
  class scgi_request_acceptor_service;

 namespace common {
 
    /// Common Traits for SCGI.
    template<>
    struct protocol_traits<tags::scgi>
    {
      /// Identity types
      typedef protocol_traits<tags::scgi>            type;
      typedef tags::scgi                             protocol_type;

      /// Char traits
      typedef char                                   char_type;
      typedef std::basic_string<char_type>           string_type;
      typedef string_type                            buffer_type;

      // Core types
      typedef basic_protocol_service<
                  protocol_type
              >                                      protocol_service_type;
      typedef basic_request<protocol_type>           request_type; 
      typedef basic_client<protocol_type>            client_type;
      typedef basic_connection<
                  tags::tcp_socket
              >                                      connection_type;

      // Runtime options
      static const parse_options parse_opts = common::parse_none;

      // Utility types
#ifdef BOOST_CGI_ENABLE_SESSIONS
      typedef basic_session<
                std::map<string_type, string_type>
              >                                      session_type;
      typedef boost::uuids::random_generator         uuid_generator_type;
      typedef basic_session_manager<
                protocol_type
              >                                      session_manager_type;
      static const bool auto_start_session = true;
      static const int session_options = path_session;
#endif // BOOST_CGI_ENABLE_SESSIONS

      // Internal types (advanced)
      typedef scgi::scgi_request_service<
                  protocol_type
              >                                      request_service_type;
      typedef scgi_request_acceptor_service<
                  protocol_type
              >                                      acceptor_service;
      typedef boost::asio::ip::tcp                   native_protocol_type;
      typedef boost::asio::socket_acceptor_service<
                  native_protocol_type
              >                                      acceptor_service_type;
      typedef 
          acceptor_service_type::implementation_type acceptor_impl_type;
      typedef acceptor_service_type::native_type     native_type;
      typedef native_protocol_type::endpoint         endpoint_type;
      typedef form_parser                            form_parser_type;
      typedef boost::none_t                          header_type;

      typedef boost::array<unsigned char, 8>         header_buffer_type;
      typedef boost::asio::const_buffers_1           const_buffers_type;
      typedef boost::asio::mutable_buffers_1         mutable_buffers_type;
    };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_SCGI_TRAITS_HPP_INCLUDED_
