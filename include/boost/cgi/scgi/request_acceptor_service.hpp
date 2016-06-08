//          -- scgi/scgi_request_acceptor_service.hpp --
//
//           Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED__
#define CGI_SCGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/version.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/scgi/request.hpp"
#include "boost/cgi/scgi/error.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  //namespace scgi {

  /// The service_impl class for FCGI basic_request_acceptor<>s
   template<typename Protocol = common::tags::scgi>
   class scgi_request_acceptor_service
     : public detail::service_base<
         ::BOOST_CGI_NAMESPACE::scgi_request_acceptor_service<Protocol>
       >
   {
   public:
   
     typedef scgi_request_acceptor_service<Protocol> self_type;
     typedef Protocol                               protocol_type;
     typedef common::protocol_traits<Protocol>      traits;
     typedef typename traits::protocol_service_type protocol_service_type;
     typedef typename traits::native_protocol_type  native_protocol_type;
     typedef typename traits::native_type           native_type;
     typedef typename traits::request_type          request_type;
     typedef typename traits::acceptor_service_type acceptor_service_type;
     typedef typename traits::acceptor_impl_type    acceptor_impl_type;
     typedef typename traits::endpoint_type         endpoint_type;
     typedef boost::shared_ptr<request_type>        request_ptr;
     typedef std::pair<
       typename std::set<request_ptr>::iterator, bool> request_iter;
     typedef boost::function<int (request_type&)>   accept_handler_type;

     struct implementation_type
     {
       typedef Protocol                               protocol_type;
       typedef common::protocol_traits<Protocol>      traits;
       typedef typename traits::protocol_service_type protocol_service_type;
       typedef typename traits::native_protocol_type  native_protocol_type;
       typedef typename traits::request_type          request_type;
       typedef typename traits::acceptor_service_type acceptor_service_type;
       typedef typename traits::endpoint_type         endpoint_type;

       acceptor_impl_type                            acceptor_;
       protocol_service_type*                        protocol_service_;
       endpoint_type                                 endpoint_;
     };

     explicit scgi_request_acceptor_service(::BOOST_CGI_NAMESPACE::common::io_service& ios)
       : detail::service_base< ::BOOST_CGI_NAMESPACE::scgi_request_acceptor_service<Protocol> >(ios)
       , acceptor_service_(boost::asio::use_service<acceptor_service_type>(ios))
       , strand_(ios)
     {
     }

     protocol_service_type&
       service(implementation_type const& impl) const
     {
       return *impl.protocol_service_;
     }

     void set_protocol_service(implementation_type& impl
                              , protocol_service_type& ps)
     {
       impl.protocol_service_ = &ps;
     }

     protocol_service_type&
       get_protocol_service(implementation_type& impl)
     {
       BOOST_ASSERT(impl.protocol_service_ != NULL);
       return *impl.protocol_service_;
     }

     void construct(implementation_type& impl)
     {
       acceptor_service_.construct(impl.acceptor_);
     }

     void destroy(implementation_type& impl)
     {
       // close/reject all the waiting requests
       acceptor_service_.destroy(impl.acceptor_);
     }

#if BOOST_VERSION >= 104700
     void shutdown_service()
     {
#if BOOST_VERSION < 104900
       acceptor_service_.shutdown_service();
#endif
     }
#endif

     boost::system::error_code
     default_init(implementation_type& impl, boost::system::error_code& ec)
     {
#if BOOST_VERSION < 104400
       ec = boost::system::error_code(829, boost::system::system_category);
#else
       ec = boost::system::error_code(829, boost::system::system_category());
#endif
       return ec;
     }

     /// Check if the given implementation is open.
     bool is_open(implementation_type& impl)
     {
       return acceptor_service_.is_open(impl.acceptor_);
     }

     /// Open a new *socket* acceptor implementation.
     boost::system::error_code
       open(implementation_type& impl, const native_protocol_type& protocol
           , boost::system::error_code& ec)
     {
       return acceptor_service_.open(impl.acceptor_, protocol, ec);
     }

     template<typename Endpoint>
     boost::system::error_code
       bind(implementation_type& impl, const Endpoint& endpoint
           , boost::system::error_code& ec)
     {
       acceptor_service_.set_option(impl.acceptor_,
           boost::asio::socket_base::reuse_address(true), ec);
       return acceptor_service_.bind(impl.acceptor_, endpoint, ec);
     }

     /// Assign an existing native acceptor to a *socket* acceptor.
     boost::system::error_code
       assign(implementation_type& impl, const native_protocol_type& protocol
             , const native_type& native_acceptor
             , boost::system::error_code& ec)
     {
       return acceptor_service_.assign(impl.acceptor_, protocol
                                      , native_acceptor, ec);
     }

     boost::system::error_code
       listen(implementation_type& impl, int backlog, boost::system::error_code& ec)
     {
       return acceptor_service_.listen(impl.acceptor_, backlog, ec);
     }
     
     void do_accept(implementation_type& impl
             , accept_handler_type handler)
     {
       request_ptr new_request;
       
       new_request = request_type::create(*impl.protocol_service_);
       
       if (!new_request->is_open())
       {
         acceptor_service_.async_accept(impl.acceptor_,
             new_request->client().connection()->next_layer(), 0,
             strand_.wrap(
               boost::bind(&self_type::handle_accept
                , this, boost::ref(impl), new_request, handler, _1
               )
             )
           );
       }
       else
       {
         impl.protocol_service_->post(
           strand_.wrap(
             boost::bind(&self_type::handle_accept
                 , this, boost::ref(impl), new_request, handler, boost::system::error_code()
               )
             )
           );
       }
     }

     void handle_accept(
         implementation_type& impl, request_ptr new_request,
         accept_handler_type handler, const boost::system::error_code& ec
      )
     {
       new_request->status(common::accepted);
       int status = handler(*new_request);
       if (new_request->is_open()) {
         new_request->close(http::ok, status);
       }
       new_request->clear();
     }

     /// Accepts a request and runs the passed handler.
     void async_accept(implementation_type& impl
             , accept_handler_type handler)
     {
       strand_.post(
         boost::bind(&self_type::do_accept,
             this, boost::ref(impl), handler)
         );
     }
     
     int accept(implementation_type& impl, accept_handler_type handler
             , endpoint_type* endpoint, boost::system::error_code& ec)
     {
       request_ptr new_request;
       
       // Accepting on new request.
       new_request = request_type::create(*impl.protocol_service_);
       
       // The waiting request may be open if it is a multiplexed request.
       if (!new_request->is_open())
       {
         ec = acceptor_service_.accept(impl.acceptor_,
                  new_request->client().connection()->next_layer(), endpoint, ec);
       }
       new_request->status(common::accepted);
       int status = handler(*new_request);
       
       if (new_request->is_open()) {
         new_request->close(common::http::ok, status);
       }
       new_request->clear();
       
       return status;
     }

     /// Accepts one request.
     template<typename CommonGatewayRequest>
     boost::system::error_code
       accept(implementation_type& impl, CommonGatewayRequest& request
             , endpoint_type* endpoint, boost::system::error_code& ec)
     {
       BOOST_ASSERT
       ( ! request.is_open()
        && "Error: Calling accept on open request (close it first?)."
       );

       // If the client is open, make sure the request is clean.
       // ie. don't leak data from one request to another!
       if (request.client().is_open())
       {
         request.clear();
       }

       // ...otherwise accept a new connection.
       ec = acceptor_service_.accept(impl.acceptor_,
                request.client().connection()->next_layer(), endpoint, ec);
       if (!ec)
         request.status(common::accepted);
       return ec;
     }

     /// Asynchronously accepts one request.
     template<typename Handler>
     void async_accept(implementation_type& impl
					  , typename implementation_type::request_type& request
                      , Handler handler)
     {
       acceptor_service_.async_accept(impl.acceptor_,
          request.client().connection()->next_layer(), handler);
     }

     /// Close the acceptor (not implemented yet).
     boost::system::error_code
       close(implementation_type& impl, boost::system::error_code& ec)
     {
#if BOOST_VERSION < 104400
       return boost::system::error_code(348, boost::system::system_category);
#else
       return boost::system::error_code(348, boost::system::system_category());
#endif
     }

     typename implementation_type::endpoint_type
       local_endpoint(implementation_type& impl, boost::system::error_code& ec)
     {
       return acceptor_service_.local_endpoint(impl.acceptor_, ec);
     }

     native_type
     native(implementation_type& impl)
     {
       return acceptor_service_.native(impl.acceptor_);
     }

     bool
     is_cgi(implementation_type& impl)
     {
       return false;
     }

   public:
     /// The underlying socket acceptor service.
     acceptor_service_type&          acceptor_service_;
     boost::asio::io_service::strand strand_;
   };

 //} // namespace scgi
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_SCGI_REQUEST_ACCEPTOR_SERVICE_HPP_INCLUDED__
