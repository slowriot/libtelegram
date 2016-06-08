//             -- fcgi/request_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_REQUEST_SERVICE_HPP_INCLUDED__
#define CGI_FCGI_REQUEST_SERVICE_HPP_INCLUDED__

#include <boost/version.hpp>
#include <boost/fusion/support.hpp>
#include <boost/system/error_code.hpp>
#include <boost/fusion/include/vector.hpp>
////////////////////////////////////////////////////////////////
#include "boost/cgi/common/form_parser.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/common/request_base.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/connections/shareable_tcp_socket.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/fcgi/client.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/import/read.hpp"
#include "boost/cgi/import/io_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace fcgi {
 
  /// The IoObjectService class for a FCGI basic_request<>s
  template<typename Protocol>
  class fcgi_request_service
    : public common::request_base<Protocol>
    , public detail::service_base<fcgi_request_service<Protocol> >
  {
  public:
    typedef fcgi_request_service<Protocol>           self_type;
    typedef fcgi_request_service<Protocol>           full_type;
    typedef common::request_base<Protocol>           base_type;
    typedef typename base_type::traits               traits;
    typedef typename base_type::client_type          client_type;
    typedef typename base_type::string_type          string_type;
    typedef typename base_type::mutable_buffers_type mutable_buffers_type;
    typedef typename traits::header_buffer_type      header_buffer_type;

    /// The actual implementation date for an FCGI request.
    struct implementation_type
      : base_type::impl_base
    {
      typedef typename base_type::traits                 traits;
      typedef typename base_type::client_type            client_type;
      typedef typename base_type::buffer_type            buffer_type;
      typedef typename base_type::mutable_buffers_type   mutable_buffers_type;
      typedef typename client_type::header_buffer_type   header_buffer_type;
      typedef spec_detail::Header                        header_type;

      implementation_type()
        : id_(0)
        , async_requests_(1)
        , request_role_(spec_detail::NONE)
      {
      }

      boost::uint16_t id_;
      boost::uint16_t async_requests_;

      fcgi::spec_detail::role_types request_role_;

      header_buffer_type header_buf_;
      header_type header_;
      
      // Buffer to hold param records and filter data, etc.
      buffer_type param_buffer_;

      mutable_buffers_type prepare_misc(std::size_t size)
      {
        // Make sure we're not trying to make a zero-sized buffer.
        BOOST_ASSERT(size && "Attempting to allocate a zero-sized buffer.");
        std::size_t bufsz(param_buffer_.size());
        param_buffer_.resize(bufsz + size);
        return boost::asio::buffer(&param_buffer_[bufsz], size);
      }
     };

    fcgi_request_service(::BOOST_CGI_NAMESPACE::common::io_service& ios)
      : detail::service_base<fcgi_request_service<Protocol> >(ios)
      , strand_(ios)
    {
    }

    ~fcgi_request_service()
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
        http::status_code hsc = http::ok
      , int program_status = 0);

    /// Close the request.
    int close(implementation_type& impl,
        http::status_code hsc
      , int program_status, boost::system::error_code& ec);

    /// Clear all request data (object is then safe to reuse).
    void clear(implementation_type& impl);

    /// Load the request to a point where it can be usefully used.
    /**
     * FastCGI:
     * --------
     *
     *  - Calls client::construct() to claim a request id from the server.
     *  - Reads, parses and handles all packets until the closing `PARAMS`
     *    packet for this request arrives from the server.
     *
     */

    boost::system::error_code
    load(implementation_type& impl, common::parse_options opts
          , boost::system::error_code& ec);

    // **FIXME**
    template<typename Handler>
    void async_load(implementation_type& impl, common::parse_options opts, Handler handler);

    /// Returns true if the request environment params have been read.
    bool params_read(implementation_type& impl);

    common::role_type role(implementation_type const& impl) const
    {
      return static_cast<common::role_type>(impl.request_role_);
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


    void handle_read_header(
        boost::system::error_code&
      );

    template<typename Handler>
    void handle_read_header(
        implementation_type& impl, 
        ::BOOST_CGI_NAMESPACE::common::parse_options opts,
        Handler handler,
        boost::system::error_code& ec,
        const std::size_t bytes_transferred
      );

    template<typename Handler>
    void handle_begin_request_header(
        implementation_type& impl,
        common::parse_options opts,
        Handler handler,
        boost::system::error_code& ec
      );

    /// Asynchronously read a single header, but do nothing with it.
    template<typename Handler>
    void async_read_header(
        implementation_type& impl,
        Handler handler)
    {
      // clear the header first (might be unneccesary).
      impl.header_buf_ = implementation_type::header_buffer_type();
      async_read(*impl.client_.connection(), buffer(impl.header_buf_), handler);
    }

    template<typename Handler>
    void async_read_header(
        implementation_type& impl,
        common::parse_options opts,
        Handler handler,
        boost::system::error_code& ec);

  protected:
    /// Read and parse the cgi POST meta variables (greedily)
    boost::system::error_code&
    parse_post_vars(implementation_type& impl, boost::system::error_code& ec);

    /// Read and parse a single cgi POST meta variable (greedily)
    template<typename RequestImpl>
    boost::system::error_code&
    parse_one_post_var(implementation_type& impl, boost::system::error_code& ec);

  /***************************************************************************/
  public: // Reading stuff goes under here

    /// Read a single header, buf do nothing with it.
    boost::system::error_code
      read_header(implementation_type& impl, boost::system::error_code& ec);

    /*** Various handlers go below here; they might find a
     * better place to live ***/

    boost::system::error_code
      handle_admin_request(implementation_type& impl, boost::system::error_code& ec);

    // **FIXME**
    void handle_other_request_header(implementation_type& impl);

    // **FIXME**    
    boost::system::error_code
      process_begin_request(implementation_type& impl, boost::uint16_t id
                           , const unsigned char* buf, std::size_t len
                           , boost::system::error_code& ec);

    boost::system::error_code
      process_abort_request(implementation_type& impl, boost::uint16_t id
                           , const unsigned char* buf, std::size_t len
                           , boost::system::error_code& ec);

    boost::system::error_code
      process_params(implementation_type& impl, boost::uint16_t id
                    , const unsigned char* buf, std::size_t len
                    , boost::system::error_code& ec);

    boost::system::error_code
      process_stdin(implementation_type& impl, boost::uint16_t id
                   , const unsigned char* buf, std::size_t len
                   , boost::system::error_code& ec);

    boost::system::error_code
      process_get_values(implementation_type& impl, boost::uint16_t id
                   , boost::system::error_code& ec);


    /// Parse the current header
    boost::tribool parse_header(implementation_type& impl);

    /// Read in the FastCGI (env) params
    boost::system::error_code
    read_env_vars(implementation_type& impl, boost::system::error_code& ec);

    boost::system::error_code
      parse_packet(implementation_type& impl, boost::system::error_code& ec);
      
    /// Read the body of the current packet; do nothing with it.
    template<typename MutableBuffersType>
    boost::system::error_code
      read_body(implementation_type& impl, const MutableBuffersType& buffer
               , boost::system::error_code& ec);

    template<typename MutableBuffersType>
    boost::system::error_code
      parse_body(implementation_type& impl, const MutableBuffersType& buffer
                , boost::system::error_code& ec);

    boost::system::error_code
      begin_request_helper(implementation_type& impl
                          , header_buffer_type& header
                          , boost::system::error_code& ec);
                          
  private:
    boost::asio::io_service::strand strand_;
  };

 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/fcgi/request.hpp"
#include "boost/cgi/basic_request.hpp"

#if !defined( BOOST_CGI_BUILD_LIB )
#    include "boost/cgi/impl/fcgi_request_service.ipp"
#endif

#endif // CGI_FCGI_REQUEST_SERVICE_HPP_INCLUDED__

