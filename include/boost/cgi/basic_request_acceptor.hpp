//            -- basic_request_acceptor.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_REQUEST_ACCEPTOR_HPP_INCLUDED__
#define CGI_BASIC_REQUEST_ACCEPTOR_HPP_INCLUDED__

#include <boost/noncopyable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/basic_io_object.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/tcp.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/common/basic_protocol_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// The interface class for any *BOOST_CGI_NAMESPACE::acceptor.
  template<typename Protocol>
  class basic_request_acceptor
    //: public boost::asio::basic_io_object<RequestAcceptorService>
    : public boost::asio::basic_io_object<
        typename protocol_traits<Protocol>::acceptor_service
      >
    //, private boost::noncopyable
  {
  public:
    //  typedef impl_type;
    typedef Protocol                               protocol_type;
    typedef protocol_traits<protocol_type>         traits;
    typedef typename traits::acceptor_service      service_type;
    typedef typename traits::endpoint_type         endpoint_type;
    typedef typename traits::native_handle_type    native_handle_type;
    typedef typename traits::protocol_service_type protocol_service_type;

    typedef typename
        service_type::accept_handler_type          accept_handler_type;

    template<typename IoContextProvider>
    explicit basic_request_acceptor(
          common::basic_protocol_service<protocol_type, IoContextProvider>& ps)
      : boost::asio::basic_io_object<service_type>(ps.get_io_context())
    {
      this->get_service().set_protocol_service(this->get_implementation(), ps);

      boost::system::error_code ec;
      if (this->get_service().default_init(this->get_implementation(), ec)) {
        detail::throw_error(ec);
      }
    }

    template<typename IoContextProvider, typename InternetProtocol>
    explicit basic_request_acceptor(
          common::basic_protocol_service<protocol_type, IoContextProvider>& ps,
          const boost::asio::ip::basic_endpoint<InternetProtocol>& endpoint,
          bool reuse_addr = true)
      : boost::asio::basic_io_object<service_type>(ps.get_io_context())
    {
      this->get_service().set_protocol_service(this->get_implementation(), ps);

      boost::system::error_code ec;
      open(endpoint.protocol(), ec);
      detail::throw_error(ec);
      if (reuse_addr)
      {
        set_option(boost::asio::socket_base::reuse_address(true), ec);
        detail::throw_error(ec);
      }
      bind(endpoint, ec);
      detail::throw_error(ec);
      listen(boost::asio::socket_base::max_listen_connections, ec);
      detail::throw_error(ec);
    }

    template<typename IoContextProvider, typename InternetProtocol>
    explicit basic_request_acceptor(
          common::basic_protocol_service<protocol_type, IoContextProvider>& ps,
          const InternetProtocol& ip,
          const native_handle_type& native_acceptor)
      : boost::asio::basic_io_object<service_type>(ps.get_io_context())
    {
      this->get_service().set_protocol_service(this->get_implementation(), ps);
      boost::system::error_code ec;
      this->get_service().assign(this->get_implementation(), ip, native_acceptor, ec);
      detail::throw_error(ec);
    }

    ~basic_request_acceptor()
    {
    }

    protocol_service_type& protocol_service() const
    {
      return this->get_service().protocol_service(this->get_implementation());
    }

    /// Check if the acceptor is open
    bool is_open()
    {
      return this->get_service().is_open(this->get_implementation());
    }

    /// Open the acceptor
    template<typename InternetProtocol>
    void open(const InternetProtocol& protocol)
    {
      boost::system::error_code ec;
      this->get_service().open(this->get_implementation(), protocol, ec);
      detail::throw_error(ec);
    }

    /// Open the acceptor
    template<typename InternetProtocol>
    boost::system::error_code
      open(const InternetProtocol& protocol, boost::system::error_code& ec)
    {
      return this->get_service().open(this->get_implementation(), protocol, ec);
    }

    /// Set the acceptor to listen
    void listen(int backlog = boost::asio::socket_base::max_listen_connections)
    {
      boost::system::error_code ec;
      this->get_service().listen(this->get_implementation(), backlog, ec);
      detail::throw_error(ec);
    }

    /// Set the acceptor to listen
    boost::system::error_code
      listen(int backlog, boost::system::error_code& ec)
    {
      return this->get_service().listen(this->get_implementation(), backlog, ec);
    }

    template<typename Endpoint>
    void bind(Endpoint& ep)
    {
      boost::system::error_code ec;
      this->get_service().bind(this->get_implementation(), ep, ec);
      detail::throw_error(ec);
      this->get_implementation().endpoint_ = ep;
    }

    template<typename Endpoint>
    boost::system::error_code
      bind(Endpoint& ep, boost::system::error_code& ec)
    {
      ec = this->get_service().bind(this->get_implementation(), ep, ec);
      this->get_implementation().endpoint_ = ep;
      return ec;
    }

    /// Set socket option
    template<typename SettableSocketOption>
    void set_option(const SettableSocketOption& option)
    {
      boost::system::error_code ec;
      this->get_service().set_option(this->get_implementation(), option, ec);
      detail::throw_error(ec);
    }

    /// Set socket option
    template<typename SettableSocketOption>
    boost::system::error_code
      set_option(const SettableSocketOption& option, boost::system::error_code& ec)
    {
      return this->get_service().set_option(this->get_implementation(), option, ec);
    }

    /// Cancel all asynchronous operations associated with the acceptor.
    boost::system::error_code
      cancel()
    {
      return this->get_service().cancel(this->get_implementation());
    }

    /// Close the acceptor
    void close()
    {
      boost::system::error_code ec;
      this->get_service().close(this->get_implementation(), ec);
      detail::throw_error(ec);
    }

    /// Close the acceptor
    boost::system::error_code
      close(boost::system::error_code& ec)
    {
      return this->get_service().close(this->get_implementation(), ec);
    }

    template<typename InternetProtocol>
    void assign(InternetProtocol protocol, const native_handle_type& native_acceptor)
    {
      boost::system::error_code ec;
      this->get_service().assign(this->get_implementation(), protocol, native_acceptor, ec);
      detail::throw_error(ec);
    }

    template<typename InternetProtocol>
    boost::system::error_code
      assign(InternetProtocol protocol, const native_handle_type& native_acceptor
            , boost::system::error_code& ec)
    {
      return this->get_service().assign(this->get_implementation(), protocol
                                 , native_acceptor, ec);
    }

    /// Accept one request and handle it with `handler`.
    int accept(accept_handler_type handler)
    {
      boost::system::error_code ec;
      int status = this->get_service().accept(this->get_implementation(), handler, &this->get_implementation().endpoint_, ec);
      detail::throw_error(ec);
      return status;
    }

    int accept(accept_handler_type handler, boost::system::error_code& ec)
    {
      return this->get_service().accept(this->get_implementation(), handler, &this->get_implementation().endpoint_, ec);
    }

    void async_accept(accept_handler_type handler)
    {
      this->get_service().async_accept(this->get_implementation(), handler);
    }

    template<typename CommonGatewayRequest>
    void accept(CommonGatewayRequest& request)
    {
      boost::system::error_code ec;
      this->get_service().accept(this->get_implementation(), request, &this->get_implementation().endpoint_, ec);
      detail::throw_error(ec);
    }

    /// Accept one request
    template<typename CommonGatewayRequest>
    boost::system::error_code
      accept(CommonGatewayRequest& request, boost::system::error_code& ec)
    {
      return this->get_service().accept(this->get_implementation(), request, &this->get_implementation().endpoint_, ec);
    }

    template<typename CommonGatewayRequest>
    boost::system::error_code
      accept(CommonGatewayRequest& request, endpoint_type& ep
            , boost::system::error_code& ec)
    {
      return this->get_service().accept(this->get_implementation(), request, &ep, ec);
    }

    /// Asynchronously accept one request
    template<typename CommonGatewayRequest, typename Handler>
    void async_accept(CommonGatewayRequest& request, Handler handler)
    {
      this->get_service().async_accept(this->get_implementation(), request, handler);
    }

    endpoint_type
      local_endpoint()
    {
      boost::system::error_code ec;
      endpoint_type ep(this->get_service().local_endpoint(this->get_implementation(), ec));
      detail::throw_error(ec);
      return ep;
    }

    endpoint_type
      local_endpoint(boost::system::error_code& ec) const
    {
      return this->get_service().local_endpoint(this->get_implementation(), ec);
    }

    native_handle_type
    native()
    {
      return this->get_service().native(this->get_implementation());
    }

    bool is_cgi()
    {
      return this->get_service().is_cgi(this->get_implementation());
    }
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_BASIC_REQUEST_ACCEPTOR_HPP_INCLUDED__

