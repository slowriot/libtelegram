//       -- fcgi/win32_acceptor_service_impl.hpp --
//
//         Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_WIN32_ACCEPTOR_SERVICE_IMPL_HPP_INCLUDED_
#define CGI_FCGI_WIN32_ACCEPTOR_SERVICE_IMPL_HPP_INCLUDED_

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
#include "boost/cgi/fcgi/error.hpp"
#include "boost/cgi/fcgi/request.hpp"
#include "boost/cgi/fcgi/transport_selection.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"

BOOST_CGI_NAMESPACE_BEGIN

   namespace detail {

     /// Helper functions for async_accept operation.
     template<typename T, typename Handler>
     struct accept_handler
     {
       accept_handler(T& t, typename T::implementation_type& impl
                     , typename T::implementation_type::request_type& req
                     , Handler& hnd)
         : type(t)
         , implementation(impl)
         , request(req)
         , handler(hnd)
       {}

       void operator()()
       {
         type.check_for_waiting_request(implementation, request, handler);
       }

       T& type;
       typename T::implementation_type& implementation;
       typename T::implementation_type::request_type& request;
       Handler handler;
     };

    // inline bool default_init_pipe(HANDLE &listen_handle, boost::system::error_code &ec)
    // {
    //   bool is_cgi = false;


    //   if((::GetStdHandle(STD_OUTPUT_HANDLE) == INVALID_HANDLE_VALUE) &&
    //      (::GetStdHandle(STD_ERROR_HANDLE)  == INVALID_HANDLE_VALUE) &&
    //      (::GetStdHandle(STD_INPUT_HANDLE)  != INVALID_HANDLE_VALUE) )
    //   {
    //     DWORD pipe_mode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT;
    //     HANDLE stdin_handle = ::GetStdHandle(STD_INPUT_HANDLE);

    //     if (!::DuplicateHandle(::GetCurrentProcess(), stdin_handle,
    //                            ::GetCurrentProcess(), &listen_handle,
    //                            0, TRUE, DUPLICATE_SAME_ACCESS))
    //       ec = fcgi::error::unable_to_duplicate_handle;
    //     else
    //     if (!::SetStdHandle(STD_INPUT_HANDLE, listen_handle))
    //       ec = fcgi::error::failed_to_redirect_stdin;
    //     else
    //     {
    //       ::CloseHandle(stdin_handle);
    //       if (::SetNamedPipeHandleState(listen_handle, &pipe_mode, NULL, NULL))
    //       {
    //         // Synchronous pipe.
    //         is_cgi = false;
    //       }
    //       else
    //       {
    //         // error, a TCP socket is being used, which isn't supported
    //         // on Windows for now.
    //         is_cgi = false;
    //         ec = fcgi::error::unsupported_handle_type;
    //       }
    //     }
    //   }
    //   else
    //     is_cgi = true;

    //   return is_cgi;
    //}
     
    template<typename Connection>
    boost::system::error_code
    accept_named_pipe(HANDLE& listen_handle, Connection& connection, boost::system::error_code& ec)
    {
        OVERLAPPED overlapped;
        ::ZeroMemory(&overlapped, sizeof(overlapped)); 
        overlapped.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL); 
        if (!overlapped.hEvent) 
#if BOOST_VERSION < 104400
            ec = boost::system::error_code(::GetLastError(), boost::system::system_category); 
#else
            ec = boost::system::error_code(::GetLastError(), boost::system::system_category()); 
#endif
        if (! ::ConnectNamedPipe(listen_handle, &overlapped)) 
        { 
          switch(::GetLastError())
          {
          case ERROR_IO_PENDING:
            if (::WaitForSingleObject(overlapped.hEvent, INFINITE) == WAIT_FAILED) 
            { 
              ::CloseHandle(overlapped.hEvent); 
#if BOOST_VERSION < 104400
              ec = boost::system::error_code(::GetLastError(), boost::system::system_category); 
#else
              ec = boost::system::error_code(::GetLastError(), boost::system::system_category()); 
#endif
            } 
            break;

          case ERROR_PIPE_CONNECTED:
            // ok, a valid connection.
            break;

          default:
            ::CloseHandle(overlapped.hEvent); 
#if BOOST_VERSION < 104400
            ec = boost::system::error_code(::GetLastError(), boost::system::system_category); 
#else
            ec = boost::system::error_code(::GetLastError(), boost::system::system_category()); 
#endif
          }
        }
        ::CloseHandle(overlapped.hEvent);

        connection.pipe_->assign(listen_handle);
        if (!connection.is_open()) 
          ::DisconnectNamedPipe(listen_handle);

      return ec;
    }
   } // namespace detail

  namespace fcgi {

  /// The service_impl class for FCGI basic_request_acceptor<>s
   /**
    * Note: this is near enough to being generic. It will hopefully translate
    *       directly to the fcgi_acceptor_service_impl. In other words you
    *       would then have one acceptor_service_impl<>, so you'd use
    *       acceptor_service_impl<scgi> acceptor_service_impl_; // and
    *       acceptor_service_impl<fcgi> acceptor_service_impl_; // etc...
    *
    * Note: If the protocol is an asynchronous protocol, which means it
    * requires access to a boost::asio::io_service instance, then this
    * class becomes a model of the Service concept (**LINK**) and must
    * only use the constructor which takes a ProtocolService (**LINK**).
    * If the protocol isn't async then the class can be used without a
    * ProtocolService.
    */
   template<typename Protocol = common::tags::fcgi>
   class win32_acceptor_service_impl
     : public detail::service_base<
         ::BOOST_CGI_NAMESPACE::fcgi::win32_acceptor_service_impl<Protocol>
       >
   {
   public:
   
     typedef win32_acceptor_service_impl<Protocol>  self_type;
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
       boost::mutex                                  mutex_;
       std::queue<boost::shared_ptr<request_type> >  waiting_requests_;
       std::set<request_ptr>                         running_requests_;
       protocol_service_type*                        service_;
       endpoint_type                                 endpoint_;
       boost::uint16_t                               async_accepts_;
       boost::system::error_code                     error_code_;

       implementation_type() : async_accepts_(0) {}
     };

     explicit win32_acceptor_service_impl(::BOOST_CGI_NAMESPACE::common::io_service& ios)
       : detail::service_base< ::BOOST_CGI_NAMESPACE::fcgi::win32_acceptor_service_impl<Protocol> >(ios)
       , acceptor_service_(boost::asio::use_service<acceptor_service_type>(ios))
       , strand_(ios)
       , transport_(detail::transport_type())
       , listen_handle_(INVALID_HANDLE_VALUE)
       , is_cgi_(false)
     {
     }

     protocol_service_type&
       service(implementation_type const& impl) const
     {
       return *impl.service_;
     }

     /// Default-initialize the acceptor.
     boost::system::error_code
       default_init(implementation_type& impl, boost::system::error_code& ec)
     {
         is_cgi_ = false;
         if (transport_ == detail::transport::pipe)
         {
           listen_handle_ = detail::stream_handle(ec);
         }
         else
         {
           impl.endpoint_ = detail::detect_endpoint();
           acceptor_service_.assign(impl.acceptor_, impl.endpoint_.protocol(), detail::socket_handle(), ec);
         }

       return ec;
     }

     void set_protocol_service(implementation_type& impl
                              , protocol_service_type& ps)
     {
       impl.protocol_service_ = &ps;
     }

     protocol_service_type& get_protocol_service(implementation_type& impl)
     {
       BOOST_ASSERT(impl.service_ != NULL);
       return *impl.service_;
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
       //if (transport_ == detail::transport::socket)
       //  acceptor_service_.shutdown_service();
     }
#endif

     /// Check if the given implementation is open.
     bool is_open(implementation_type& impl)
     {
       if (transport_ == detail::transport::pipe)
         return listen_handle_ != INVALID_HANDLE_VALUE;
       else // transport_ == detail::transport::socket
         acceptor_service_.is_open(impl.acceptor_);
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
       
       if (impl.waiting_requests_.empty())
       {
         // Accepting on new request.
         new_request = request_type::create(*impl.service_);
       }
       else
       {
         // Accepting on existing request.
         new_request = impl.waiting_requests_.front();
         impl.waiting_requests_.pop();
       }
       new_request->async_requests() = impl.async_accepts_;
       impl.running_requests_.insert(new_request);
       
       // The waiting request may be open if it is a multiplexed request.
       // If we can reuse this request's connection, return.
       if (!new_request->is_open() && !new_request->client().keep_connection())
       {
         // ...otherwise accept a new connection.
         if (transport_ == detail::transport::pipe)
         {
            boost::system::error_code ec;
            detail::accept_named_pipe(listen_handle_, *new_request->client().connection(), ec);
            if (!ec)
              strand_.post(boost::bind(&self_type::handle_accept, this, boost::ref(impl), new_request, handler, ec));
         }
         else // transport_ == detail::transport::socket
         {
           boost::system::error_code ec;
           acceptor_service_.accept(impl.acceptor_, *new_request->client().connection()->socket_, &impl.endpoint_, ec);
           if (!ec)
              strand_.post(boost::bind(&self_type::handle_accept, this, boost::ref(impl), new_request, handler, ec));
         }
       }
       else
       {
         if (transport_ == detail::transport::pipe)
         {
           strand_.post(boost::bind(&self_type::handle_accept, this, boost::ref(impl), new_request, handler, boost::system::error_code()));
         }
         else // transport_ == detail::transport::socket
         {
           impl.service_->post(strand_.wrap(boost::bind(
             &self_type::handle_accept
             , this
             , boost::ref(impl)
             , new_request
             , handler
             , boost::system::error_code())));
         }
       }
     }

     void handle_accept(
         implementation_type& impl, request_ptr new_request,
         accept_handler_type handler, const boost::system::error_code& ec
      )
     {
       if (impl.error_code_)
           return; // There is already an error, don't overwrite it.

       if (ec)
       {
           impl.error_code_ = ec;
           return;
       }

       new_request->status(common::accepted);
       int status = handler(*new_request);
       impl.running_requests_.erase(impl.running_requests_.find(new_request));
       if (new_request->is_open())
         new_request->close(http::ok, status);

       new_request->clear();
       impl.waiting_requests_.push(new_request);

       if (status != 0)
       {
         impl.error_code_ = boost::system::error_code(status, boost::system::generic_category());
         return;
       }
 
       strand_.post(
         boost::bind(&self_type::do_accept
         , this
         , boost::ref(impl)
         , handler));
     }

     /// Accepts a request and runs the passed handler.
     void async_accept(implementation_type& impl
             , accept_handler_type handler)
     {
       strand_.post(
         boost::bind(&self_type::do_accept,
             this, boost::ref(impl), handler)
         );
       ++impl.async_accepts_;
     }
     
     int accept(implementation_type& impl, accept_handler_type handler
             , endpoint_type* endpoint, boost::system::error_code& ec)
     {
       typedef typename std::set<request_ptr>::iterator iter_t;
       typedef std::pair<iter_t, bool> pair_t;
       
       request_ptr new_request;
       pair_t insert_result;
       
       if (impl.waiting_requests_.empty())
       {
         // Accepting on new request.
         new_request = request_type::create(*impl.service_);
       }
       else
       {
         // Accepting on existing request.
         new_request = impl.waiting_requests_.front();
         impl.waiting_requests_.pop();
       }
       
       insert_result = impl.running_requests_.insert(new_request);
       
       // The waiting request may be open if it is a multiplexed request.
       if (!new_request->is_open())
       {
         // If we can reuse this request's connection, return.
         if (!new_request->client().keep_connection())
         {
           // ...otherwise accept a new connection.
           if (transport_ == detail::transport::pipe)
             detail::accept_named_pipe(listen_handle_, *new_request->client().connection(), ec);
           else // transport_ == detail::transport::socket
             acceptor_service_.accept(impl.acceptor_, *new_request->client().connection()->socket_, &impl.endpoint_, ec);
         }
       }
       new_request->async_requests() = 1;
       new_request->status(common::accepted);
       int status = handler(*new_request);
       
       impl.running_requests_.erase(insert_result.first);
       if (new_request->is_open()) {
         new_request->close(http::ok, status);
       }
       new_request->clear();
       impl.waiting_requests_.push(new_request);
       
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

       // If we can reuse this request's connection, return.
       if (request.client().keep_connection())
         return ec;

       if (transport_ == detail::transport::pipe)
         detail::accept_named_pipe(listen_handle_, *request.client().connection(), ec);
       else // transport_ == detail::transport::socket
         acceptor_service_.accept(impl.acceptor_, *new_request->client().connection()->socket_, &impl.endpoint_, ec);

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
       if (transport_ == detail::transport::pipe)
         strand_.post(detail::accept_handler<self_type, Handler>(*this, impl, request, handler));
       else // transport_ == detail::transport::socket
         this->io_service().post(detail::accept_handler<self_type, Handler>(*this, impl, request, handler));
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
       if (transport_ == detail::transport::pipe)
       {
         return is_cgi_;
       }
       else // transport_ == detail::transport::socket
       {
         boost::system::error_code ec;
         socklen_t len(static_cast<socklen_t>(local_endpoint(impl,ec).capacity()));
         int check (getpeername(native(impl), local_endpoint(impl,ec).data(), &len));
         return ( check == SOCKET_ERROR && WSAGetLastError() == WSAENOTCONN ) ? false : true;
       }
     }

   public:
     template<typename CommonGatewayRequest, typename Handler>
     int check_for_waiting_request(implementation_type& impl
                                   , CommonGatewayRequest& request
                                   , Handler handler)
     {
       // We can't call accept on an open request (close it first).
       if (request.is_open())
         return handler(error::accepting_on_an_open_request);

       // If the client is open, make sure the request is clean.
       // ie. don't leak data from one request to another!
       if (request.client().is_open())
       {
         request.clear();
       }

       // If we can reuse this request's connection, return.
       if (request.client().keep_connection())
         return handler(boost::system::error_code());

       // ...otherwise accept a new connection.
       if (transport_ == detail::transport::pipe)
       {
         boost::system::error_code ec;
         detail::accept_named_pipe(listen_handle_, *request.client().connection(), ec);
         strand_.post(handler);
       }
       else // transport_ == detail::transport::socket
       {
           acceptor_service_.async_accept(impl.acceptor_, *request.client().connection()->socket_, impl.endpoint_, handler);
       }

       return 0;
     }

   public:
     /// The underlying socket acceptor service.
     acceptor_service_type&          acceptor_service_;
     boost::asio::io_service::strand strand_;
     detail::transport::type transport_;
     HANDLE listen_handle_;
     bool is_cgi_;
   };

 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_FCGI_WIN32_ACCEPTOR_SERVICE_IMPL_HPP_INCLUDED_
