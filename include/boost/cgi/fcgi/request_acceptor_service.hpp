//         -- fcgi/request_acceptor_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED
#define CGI_FCGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED
#include "boost/cgi/detail/push_options.hpp"

#include <boost/utility/enable_if.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/service_base.hpp"
#if defined(BOOST_WINDOWS)
#  include "boost/cgi/fcgi/win32_acceptor_service_impl.hpp"
#else
#  include "boost/cgi/fcgi/acceptor_service_impl.hpp"
#endif // defined(BOOST_WINDOWS)
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"
#include "boost/cgi/import/io_service.hpp"
#include <boost/version.hpp>

BOOST_CGI_NAMESPACE_BEGIN

  /// The service class for FCGI basic_request_acceptor<>s
  /**
   * Note: If the protocol is an asynchronous protocol, which means it 
   * requires access to a boost::asio::io_service instance, then this class
   * becomes a model of the Service concept (**LINK**) and must only use the
   * constructor which takes a ProtocolService (**LINK**). If the protocol
   * isn't async then the class can be used without a ProtocolService.
   */
  template<typename Protocol = common::tags::fcgi>
  class fcgi_request_acceptor_service
    : public detail::service_base<fcgi_request_acceptor_service<Protocol> >
  {
  public:
    //typedef typename service_impl_type::impl_type     impl_type;

    typedef Protocol                                    protocol_type;
    typedef common::protocol_traits<protocol_type>      traits;
    typedef typename traits::endpoint_type              endpoint_type;
    typedef typename traits::acceptor_service_type      acceptor_service_type;
    typedef typename traits::protocol_service_type      protocol_service_type;

#if defined(BOOST_WINDOWS)
    typedef fcgi::win32_acceptor_service_impl<Protocol> service_impl_type;
#else
    typedef fcgi::acceptor_service_impl<Protocol>       service_impl_type;
#endif // defined(BOOST_WINDOWS)

    typedef typename service_impl_type::implementation_type      implementation_type;
    typedef typename service_impl_type::native_type              native_type;
    typedef typename service_impl_type::accept_handler_type      accept_handler_type;

    /// The unique service identifier
    //static boost::asio::io_service::id id;

    fcgi_request_acceptor_service(common::io_service& ios)
      : detail::service_base<fcgi_request_acceptor_service<protocol_type> >(ios)
      , service_impl_(ios)
    {
    }

    void construct(implementation_type& impl)
    {
      service_impl_.construct(impl);
    }

    void destroy(implementation_type& impl)
    {
      service_impl_.destroy(impl);
    }

#if BOOST_VERSION >= 104700
    void shutdown_service()
    {
#if BOOST_VERSION < 104900
      service_impl_.shutdown_service();
#endif
    }
#endif

    protocol_service_type&
      protocol_service(implementation_type const& impl) const
    {
      return service_impl_.service(impl);
    }

    boost::system::error_code
      default_init(implementation_type& impl, boost::system::error_code& ec)
    {
      return service_impl_.default_init(impl, ec);
    }

    bool is_open(implementation_type& impl)
    {
      return service_impl_.is_open(impl);
    }

    void close(implementation_type& impl)
    {
      boost::system::error_code ec;
      service_impl_.close(impl, ec);
      detail::throw_error(ec);
    }

    boost::system::error_code
      close(implementation_type& impl, boost::system::error_code& ec)
    {
      return service_impl_.close(impl, ec);
    }

    template<typename InternetProtocol>
    boost::system::error_code
      open(implementation_type& impl, const InternetProtocol& protocol
          , boost::system::error_code& ec)
    {
      return service_impl_.open(impl, protocol, ec);
    }

    template<typename Endpoint>
    boost::system::error_code
      bind(implementation_type& impl, const Endpoint& endpoint
          , boost::system::error_code& ec)
    {
      return service_impl_.bind(impl, endpoint, ec);
    }

    boost::system::error_code
      listen(implementation_type& impl, int backlog, boost::system::error_code& ec)
    {
      return service_impl_.listen(impl, backlog, ec);
    }

    int accept(implementation_type& impl, accept_handler_type handler
            , endpoint_type * ep, boost::system::error_code& ec)
    {
      return service_impl_.accept(impl, handler, ep, ec);
    }

    void async_accept(implementation_type& impl
            , accept_handler_type handler)
    {
      return service_impl_.async_accept(impl, handler);
    }

    template<typename CommonGatewayRequest>
    boost::system::error_code
      accept(implementation_type& impl, CommonGatewayRequest& request
            , endpoint_type * ep, boost::system::error_code& ec)
    {
      return service_impl_.accept(impl, request, ep, ec);
    }

    template<typename CommonGatewayRequest, typename Handler>
    void async_accept(implementation_type& impl, CommonGatewayRequest& req
                     , Handler handler)
    {
      service_impl_.async_accept(impl, req, handler);
    }

    template<typename T>
    void set_protocol_service(implementation_type& impl, T& ps)
    {
      impl.service_ = &ps;
    }

    endpoint_type
      local_endpoint(implementation_type& impl, boost::system::error_code& ec)
    {
      return service_impl_.local_endpoint(impl, ec);
    }

    template<typename InternetProtocol>
    boost::system::error_code
      assign(implementation_type& impl, InternetProtocol protocol
            , const native_type& native_acceptor, boost::system::error_code& ec)
    {
      return service_impl_.assign(impl, protocol, native_acceptor, ec);
    }

    native_type native(implementation_type& impl)
    {
      return service_impl_.native(impl);
    }

    bool
    is_cgi(implementation_type& impl)
    {
      return service_impl_.is_cgi(impl);
    }
  public:
    service_impl_type service_impl_;
  };

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_FCGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED

