//           -- request_acceptor_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED
#define CGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED

#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/protocol_traits.hpp"
#include "boost/cgi/config.hpp"
#include <boost/version.hpp>

BOOST_CGI_NAMESPACE_BEGIN

  template<typename Protocol>
  class request_acceptor_service
    : public detail::service_base<request_acceptor_service<Protocol> >
  {
    typedef detail::protocol_traits<Protocol>::acceptor_service_impl
      service_impl_type;
  public:
    typedef typename service_impl_type::impl_type  implementation_type;
    typedef Protocol protocol_type;

    request_acceptor_service(basic_protocol_service<protocol_type>& s)
      : detail::service_base<request_acceptor_service<Protocol>(s.io_service())
    {
    }

#if BOOST_VERSION >= 104700
    void shutdown_service()
    {
    }
#endif

    void construct(implementation_type& impl)
    {
      service_impl_.construct(impl);
    }

    void destroy(implementation_type& impl)
    {
      service_impl_.destroy(impl);
    }

    /// Accept a request
    /**
     * Check if there is a waiting request in the queue. If not, accept a
     * connection, and associate it with the request.
     */
    template<typename CommonGatewayRequest>
    boost::system::error_code&
    accept(implementation_type& impl, CommonGatewayRequest& request
          , boost::system::error_code& ec)
    {
      return service_impl_.accept(impl, request, ec);
      /*      
      boost::thread::mutex::scoped_lock lk(io_service_.mutex_);
      if( !io_service_.request_queue_.empty() )
      {
	      request = pservice_.request_queue_.front();
	      pservice_.request_queue_.pop();

	      return ec;
      }
      lk.unlock();
      pservice_.gateway_.accept(&request.connection(), ec);
            
      return ec;
      */
    }

    /// Asynchronously accept a request
    template<typename CommonGatewayRequest, typename Handler>
    void async_accept(implementation_type& impl, CommonGatewayRequest& request
                     , Handler handler)
    {
      service_impl_.async_accept(impl, request, handler);
      /*
      boost::thread::mutex::scoped_lock lk(io_service_.mutex_);
      if( !io_service_.request_queue_.empty() )
      {
	      request = pservice_.request_queue_.front();
	      pservice_.request_queue_.pop();

	      return ec;
      }
      lk.unlock();
      pservice_.gateway_.async_accept(request.connection(), handler);
      */
    }

  private:
    service_impl_type& service_impl_;
    //    basic_protocol_service<protocol_type>& pservice_;
  };

BOOST_CGI_NAMESPACE_END

#endif // CGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED
