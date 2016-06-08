//             -- scgi/request_service.hpp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_SERVICE_HPP_INCLUDED__
#define CGI_SCGI_REQUEST_SERVICE_HPP_INCLUDED__

#include <boost/version.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/support.hpp>
#include <boost/system/error_code.hpp>
////////////////////////////////////////////////////////////////
#include "boost/cgi/common/form_parser.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/common/request_base.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/connections/tcp_socket.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/scgi/client.hpp"
#include "boost/cgi/scgi/request_parser.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/import/read.hpp"
#include "boost/cgi/import/io_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace scgi {
 
  /// The IoObjectService class for a SCGI basic_request<>s
  template<typename Protocol>
  class scgi_request_service
    : public common::request_base<Protocol>
    , public detail::service_base<scgi_request_service<Protocol> >
  {
  public:
    typedef scgi_request_service<Protocol>           self_type;
    typedef scgi_request_service<Protocol>           full_type;
    typedef common::request_base<Protocol>           base_type;
    typedef typename base_type::traits               traits;
    typedef typename base_type::client_type          client_type;
    typedef typename base_type::string_type          string_type;
    typedef typename base_type::mutable_buffers_type mutable_buffers_type;
    typedef typename traits::header_buffer_type      header_buffer_type;

    /// The actual implementation date for an SCGI request.
    struct implementation_type
      : base_type::impl_base
    {
      typedef typename base_type::traits                 traits;
      typedef typename base_type::client_type            client_type;
      typedef typename base_type::buffer_type            buffer_type;
      typedef typename base_type::mutable_buffers_type   mutable_buffers_type;
      typedef typename client_type::header_buffer_type   header_buffer_type;
      typedef request_parser               request_parser_type;

      implementation_type()
        : id_(0)
      {
      }

      boost::uint16_t id_;

      header_buffer_type header_buf_;
      
      // Buffer to hold param records and filter data, etc.
      buffer_type param_buffer_;
      boost::shared_ptr<boost::array<char, 8192> > buffer_;

      /// Whether the request is valid or not.
      boost::tribool valid_request_;

      /// The parser for the incoming request.
      request_parser_type request_parser_;

      mutable_buffers_type prepare_misc(std::size_t size)
      {
        // Make sure we're not trying to make a zero-sized buffer.
        BOOST_ASSERT(size && "Attempting to allocate a zero-sized buffer.");
        std::size_t bufsz(param_buffer_.size());
        param_buffer_.resize(bufsz + size);
        return boost::asio::buffer(&param_buffer_[bufsz], size);
      }
     };

     template<typename Service>
     struct callback_functor
     {
       callback_functor(implementation_type& impl, Service* service)
         : impl_(impl)
         , service_(service)
       {
       }

       std::size_t operator()(boost::system::error_code& ec)
       {
         return service_->read_some(impl_, ec);
       }

     private:
       implementation_type& impl_;
       Service* service_;
     };


    scgi_request_service(::BOOST_CGI_NAMESPACE::common::io_service& ios)
      : detail::service_base<scgi_request_service<Protocol> >(ios)
      , strand_(ios)
    {
    }

    ~scgi_request_service()
    {
    }

    void construct(implementation_type& impl)
    {
      impl.client_.set_connection(
        implementation_type::connection_type::create(this->get_io_service())
      );
    }

#if BOOST_VERSION >= 104700
    void shutdown_service()
    {
    }
#endif

    /// Close the request.
    int close(implementation_type& impl,
      common::http::status_code hsc = common::http::ok
      , int program_status = 0);

    /// Close the request.
    int close(implementation_type& impl,
        common::http::status_code hsc
      , int program_status, boost::system::error_code& ec);

    /// Clear all request data (object is then safe to reuse).
    void clear(implementation_type& impl);

    /// Load the request to a point where it can be usefully used.
    boost::system::error_code
    load(implementation_type& impl, common::parse_options opts
          , boost::system::error_code& ec);

    /// Read and parse the cgi POST meta variables (greedily)
    template<typename RequestImpl>
    boost::system::error_code
    parse_post_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Return an error, except ignore EOF, as this is expected.
      if (ec)
      {
        if (ec == boost::cgi::common::error::eof)
          ec = boost::system::error_code();
        else
          return ec;
      }

      return base_type::parse_post_vars(
          impl,
          callback_functor<self_type>(impl, this),
          ec
        );
    }

    // **FIXME**
    template<typename Handler>
    void async_load(implementation_type& impl, common::parse_options opts, Handler handler);

    /// Returns true if the request environment params have been read.
    bool params_read(implementation_type& impl);

    common::role_type role(implementation_type const& impl) const
    {
      return common::responder;
    }

    client_type&
      client(implementation_type& impl)
    {
      return impl.client_;
    }
    
  private:
  
    template<typename Handler>
    void do_load(
        implementation_type& impl, common::parse_options opts,
        Handler handler, boost::system::error_code& ec
      );

  private:
    boost::asio::io_service::strand strand_;
  };

 } // namespace scgi
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/scgi/request.hpp"
#include "boost/cgi/basic_request.hpp"

#if !defined( BOOST_CGI_BUILD_LIB )
#    include "boost/cgi/scgi/request_service.ipp"
#endif

#endif // CGI_SCGI_REQUEST_SERVICE_HPP_INCLUDED__
