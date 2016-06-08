//             -- fcgi/request_service.ipp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_REQUEST_SERVICE_IPP_INCLUDED__
#define CGI_FCGI_REQUEST_SERVICE_IPP_INCLUDED__

#include <vector>

#include <boost/fusion/support.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system/error_code.hpp>
////////////////////////////////////////////////////////////////
#include "boost/cgi/fcgi/error.hpp"
#include "boost/cgi/fcgi/client.hpp"
#include "boost/cgi/fcgi/request_service.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/form_parser.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/common/request_base.hpp"
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/import/read.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN

   namespace detail {

     /// Helper function to determine the fcgi param length of a parameter (for get values request).
     template <class String>
     std::string param_length(String const &str) {
       std::string result;
       boost::uint32_t length = boost::uint32_t(str.size());
       if (length < 128)
       {
         unsigned char c(length & 0x0000007F);
         result += reinterpret_cast<char const&>(c);
       }
       else
       {
         unsigned char c(((length & 0xFF000000) >> 24) | 0x80);
         result += reinterpret_cast<char const&>(c);
         c = ((length & 0x00FF0000) >> 16);
         result += reinterpret_cast<char const&>(c);
         c = ((length & 0x0000FF00) >> 8);
         result += reinterpret_cast<char const&>(c);
         c = (length & 0x000000FF);
         result += reinterpret_cast<char const&>(c);
       }

       return result;
     }
     
     /// Helper class to asynchronously load a request.
     /**
      * This is just a function object that can be posted to another
      * thread to do the actual work of loading. It's operator()() does
      * the work.
      */
     template<typename T, typename Handler>
     struct async_load_helper
     {
       async_load_helper(T& t, typename T::implementation_type& impl
                        , common::parse_options opts, Handler h)
         : type(t)
         , impl_(impl)
         , parse_opts_(opts)
         , handler_(h)
       {
       }

       void operator()()
       {
         boost::system::error_code ec;
         type.load(impl_, parse_opts_, ec);
         handler_(ec);
       }

       T& type;
       typename T::implementation_type& impl_;
       common::parse_options parse_opts_;
       Handler handler_;
     };
     
     struct clear_data
     {
       template<typename T>
       void operator()(T& x) const {
         x.clear();
       }
     };

  } // namespace detail

  namespace fcgi {
 
    /// Close the request.
    template<typename Protocol>
    BOOST_CGI_INLINE int
    fcgi_request_service<Protocol>::close(
        implementation_type& impl
      , http::status_code hsc
      , int program_status)
    {
      boost::system::error_code ec;
      close(impl, hsc, program_status, ec);
      detail::throw_error(ec);
      return program_status;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE int
    fcgi_request_service<Protocol>::close(
        implementation_type& impl
      , http::status_code hsc
      , int program_status
      , boost::system::error_code& ec)
    {
      /**
       * Apache on Windows with mod_fcgid requires that all of the
       * pending data for the connection is read before the request
       * is closed.
       */
      while(!ec 
        && impl.client_.status() < common::stdin_read
        && impl.request_status_ != common::loaded)
      {
        parse_packet(impl, ec);
      }

      impl.all_done_ = true;
      impl.client_.close(program_status, ec);
      impl.request_status_ = common::closed;
      return program_status;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE void
    fcgi_request_service<Protocol>::clear(implementation_type& impl)
    {
      impl.post_buffer_.clear();
      impl.param_buffer_.clear();
      // Clear all request data.
      boost::fusion::for_each(impl.vars_, detail::clear_data());
      impl.stdin_parsed_ = false;
      impl.http_status_ = common::http::no_content;
      impl.request_status_ = common::null;
      impl.request_role_ = spec_detail::NONE;
      impl.all_done_ = false;
      impl.client_.status_ = common::none_;
      impl.client_.request_id_ = -1;
	  impl.id_ = 0;
    }
      
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
    template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::load(
        implementation_type& impl, common::parse_options opts
      , boost::system::error_code& ec)
    {
      impl.client_.construct(impl, ec);
      // Bomb out if the client isn't open here.
      if (!impl.client_.connection()->is_open())
          return error::client_not_open;

      while(!ec)
      {
        if (read_header(impl, ec))
          break;

        int id(spec::get_request_id(impl.header_buf_));
        if (id == spec::null_request_id::value)
        {
          ec = handle_admin_request(impl, ec);
        }
        else if (impl.id_ && impl.id_ != id)
        {
          // The library doesn't support multiplexed connections yet,
          // mainly because I've never had access to a server that
          // supports it.
          //
          // If you have one, can I use it?
          ec = error::multiplexing_not_supported;
        }
        else if (spec::get_type(impl.header_buf_) == spec::begin_request::value)
        {
          impl.id_ = id;
          impl.client_.request_id_ = id;
          if (!read_header(impl, ec))
          {
            spec::begin_request packet(impl.header_buf_);
            impl.request_role_ = packet.role();
            impl.client_.keep_connection_
              = packet.flags() & spec::keep_connection;
            break;
          }
        }
        else
        {
          handle_other_request_header(impl);
        }
      }

      if (ec)
      {
        impl.client_.close();
        impl.client_.connection()->close();
        impl.request_status_ = common::closed;
        return ec;
      }
      
      if (opts & common::parse_env)
      {
        read_env_vars(impl, ec);
      }

      string_type const &request_method = env_vars(impl.vars_)["REQUEST_METHOD"];
      if (request_method == "GET")
      {
        if (common::request_base<Protocol>::parse_get_vars(impl, ec))
          return ec;
      }
      else if (request_method == "POST" && (opts & common::parse_post_only))
      {
        if (opts & common::parse_post_only)
        {
          while(!ec 
            && impl.client_.status() < common::stdin_read
            && impl.request_status_ != common::loaded)
          {
            parse_packet(impl, ec);
          }
        }
        
        if (parse_post_vars(impl, ec))
          return ec;
      }
     
      if (opts & common::parse_cookies_only)
          common::request_base<Protocol>::parse_cookie_vars(impl, "HTTP_COOKIE", ec);

      return ec;
    }
    
    template<typename Protocol>
    template<typename Handler>
    BOOST_CGI_INLINE
    void fcgi_request_service<Protocol>::do_load(
        implementation_type& impl, common::parse_options opts,
        Handler handler, boost::system::error_code& ec
      )
    {
      impl.client_.construct(impl, ec);
      // Bomb out if the client isn't open here.
      if (!impl.client_.connection()->is_open())
          ec = error::client_not_open;

    }
    
    template<typename Protocol>
    template<typename Handler>
    BOOST_CGI_INLINE
    void fcgi_request_service<Protocol>::handle_read_header(
        implementation_type& impl, 
        common::parse_options opts,
        Handler handler,
        boost::system::error_code& ec,
        const std::size_t bytes_transferred
      )
    {
      if(ec)
        handler(ec);
      else
      {
        int id(spec::get_request_id(impl.header_buf_));
        if (id == spec::null_request_id::value)
          ec = handle_admin_request(impl, ec);
        else
        if (impl.id_ && impl.id_ != id)
        {
          // The library doesn't "officially" support multiplexed
          // connections yet, because I've never had access to a server
          // that supports it.
          //
          // If you have one, can I use it?
          handler(error::multiplexing_not_supported);
        }
        else
        if (spec::get_type(impl.header_buf_) 
            == spec::begin_request::value)
        {
          impl.id_ = id;
          impl.client_.request_id_ = id;
          if (!read_header(impl, ec))
          {
            spec::begin_request packet(impl.header_buf_);
            impl.request_role_ = packet.role();
            impl.client_.keep_connection_
              = packet.flags() & spec::keep_connection;
            strand_.post(&self_type::handle_begin_request_header,
                this, boost::ref(impl), handler, _1
              );
          }
        }else
          handle_other_request_header(impl);
      }
    }
        
    template<typename Protocol>
    template<typename Handler>
    BOOST_CGI_INLINE
    void fcgi_request_service<Protocol>::handle_begin_request_header(
        implementation_type& impl,
        common::parse_options opts,
        Handler handler,
        boost::system::error_code& ec
      )
    {
      if (//impl.request_status_ < common::env_read &&
          opts & common::parse_env)
      {
        read_env_vars(impl, ec);
        //impl.request_status_ = common::env_read;
      }

      string_type const&
        request_method (env_vars(impl.vars_)["REQUEST_METHOD"]);
        
      if (request_method == "GET")
      {
          if (common::request_base<Protocol>::parse_get_vars(impl, ec))
          return;
      }
      else
      if (request_method == "POST" 
          && opts & common::parse_post_only)
      {
        //std::cerr<< "Parsing post vars now.\n";

        if (opts & common::parse_post_only)
        {
          while(!ec 
            && impl.client_.status() < common::stdin_read
            && impl.request_status_ != common::loaded)
          {
            parse_packet(impl, ec);
          }
        }
        
        if (parse_post_vars(impl, ec))
	      return;
      }
      if (opts & common::parse_cookies_only)
          common::request_base<Protocol>::parse_cookie_vars(impl, ec);
        
      if (ec == error::eof) {
        ec = boost::system::error_code();
      }
    }

    // **FIXME**
    template<typename Protocol>
    template<typename Handler> BOOST_CGI_INLINE
    void fcgi_request_service<Protocol>::async_load(
        implementation_type& impl, common::parse_options opts, Handler handler)
    {
      /*
      strand_.post(
          boost::bind(&self_type::do_load<Handler>,
              this, boost::ref(impl), opts, handler
            )
        );
      */
        
      strand_.post(
        detail::async_load_helper<self_type, Handler>(this, opts, handler)
      );
    }

    /// Check if the params have been read (ie. FCGI_PARAMS packets)
    template<typename Protocol>
    BOOST_CGI_INLINE
    bool fcgi_request_service<Protocol>::params_read(implementation_type& impl)
    {
      return impl.client_.status() >= common::params_read;
    }

    /// Read and parse the cgi POST meta variables (greedily)
    template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code&
    fcgi_request_service<Protocol>::parse_post_vars(
        implementation_type& impl
      , boost::system::error_code& ec)
    {
      impl.client_.bytes_left_
         = boost::lexical_cast<std::size_t>(
             env_vars(impl.vars_)["CONTENT_LENGTH"]);
      
      return base_type::parse_post_vars(
          impl,
          detail::callback_functor<implementation_type, self_type>(impl, this),
          ec
        );
    }

    /// Read and parse a single cgi POST meta variable (greedily)
    template<typename Protocol>
    template<typename RequestImpl> BOOST_CGI_INLINE
    boost::system::error_code&
    fcgi_request_service<Protocol>::parse_one_post_var(
        implementation_type& impl, boost::system::error_code& ec)
    {
      //#     error "Not implemented"
      return ec;
    }

    /// Read in the FastCGI (env) params
    // **FIXME**
    template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::read_env_vars(
        implementation_type& impl, boost::system::error_code& ec)
    {
      while(!ec && !(common::request_base<Protocol>::status(impl) & common::env_read))
      {
        if (this->read_header(impl, ec))
          return ec;
          
        boost::tribool state = this->parse_header(impl);

        if (state)
        { // the header has been handled and all is ok; continue.
          impl.client_.status(common::params_read);
          common::request_base<Protocol>::status(impl, (common::request_status)(common::request_base<Protocol>::status(impl) | common::env_read));
        }
        else
        if (!state)
        { // The header is confusing; something's wrong. Abort.
          ec = error::bad_header_type;
        }
        else // => (state == boost::indeterminate)
        {
          std::size_t remaining(
            fcgi::spec::get_length(impl.header_buf_));

          if (remaining)
          {
            mutable_buffers_type buf = impl.prepare_misc(remaining);

            if (this->read_body(impl, buf, ec))
              return ec;

            this->parse_body(impl, buf, ec);
          } else
            ec = error::couldnt_write_complete_packet;
        }

      } // while(!ec && !(status(impl) & common::env_read))
      return ec;
    }

    /// Read a single header, but do nothing with it.
    template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::read_header(
        implementation_type& impl
      , boost::system::error_code& ec)
    {
      // clear the header first (might be unneccesary).
      impl.header_buf_ = header_buffer_type();
      this->get_io_service().poll();

      async_read(*impl.client_.connection(), buffer(impl.header_buf_), [&](boost::system::error_code const &e, std::size_t bytes_transfered) -> void
      {
        ec = e;
      });

      while (impl.header_buf_ == header_buffer_type() && !ec)
      {
        this->get_io_service().run_one();
        if (this->get_io_service().stopped())
          ec = error::eof;
      }
      
      return ec;
    }

    template<typename Protocol>
    template<typename Handler>
    BOOST_CGI_INLINE void
    fcgi_request_service<Protocol>::async_read_header(
        implementation_type& impl
      , common::parse_options opts
      , Handler handler
      , boost::system::error_code& ec)
    {
      // clear the header first (might be unneccesary).
      impl.header_buf_ = implementation_type::header_buffer_type();

      boost::asio::async_read(
          *impl.client_.connection(), buffer(impl.header_buf_)
        , boost::asio::transfer_all()
        , strand_.wrap(
              boost::bind(&self_type::handle_read_header,
                  this, boost::ref(impl), opts, handler,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred
                )
            )
        );
    }

    /*** Various handlers go below here; they might find a
     * better place to live ***/

    template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::handle_admin_request(implementation_type& impl, boost::system::error_code& ec)
    {
      switch(fcgi::spec::get_type(impl.header_buf_))
      {
      case 9: process_get_values(impl, fcgi::spec::get_request_id(impl.header_buf_), ec);
              break;
      default: break;
      }

      return ec;
    }

    // **FIXME**
    template<typename Protocol>
    BOOST_CGI_INLINE void
    fcgi_request_service<Protocol>::handle_other_request_header(implementation_type& /* impl */)
    {
      //std::cerr<< std::endl << "**FIXME** " << __FILE__ << ":" << __LINE__ 
      //  << " handle_other_request_header()" << std::endl;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::process_abort_request(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf, std::size_t len
      , boost::system::error_code& ec)
    {
      if (id == fcgi::spec::get_request_id(impl.header_buf_))
      {
        impl.request_status_ = common::aborted;
        return ec;
      }
      try {
        //std::cerr<< "**FIXME** request aborted (id = " << id
        //  << ") but request not notified." << std::endl;
        //impl.client_.connection_->requests_.at(id - 1)->abort();
      }catch(...){
        ec = error::abort_request_record_recieved_for_invalid_request;
      }
      return ec;
    }

    template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::process_params(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf, std::size_t len
      , boost::system::error_code& ec)
    {
      if (0 == len)
      { // This is the final param record.
        
        impl.client_.status(common::params_read);
        return ec;
      }

      while(len)
      {
        boost::uint32_t   name_len, data_len;
        string_type name, data;
        if (*buf >> 7 == 0)
        {
            name_len = *(buf++);
            --len;
        }
        else
        {
            name_len = ((buf[0] & 0x7F) << 24)
                     +  (buf[1]         << 16)
                     +  (buf[2]         <<  8)
                     +   buf[3];
            buf += 4;
            len -= 4;
        }

        if (*buf >> 7 == 0)
        {
            data_len = *(buf++);
            --len;
        }
        else
        {
            data_len = ((buf[0] & 0x7F) << 24)
                     +  (buf[1]         << 16)
                     +  (buf[2]         <<  8)
                     +   buf[3];
            buf += 4;
            len -= 4;
        }
        name.assign(reinterpret_cast<const char*>(buf), name_len);
        data.assign(reinterpret_cast<const char*>(buf)+name_len, data_len);
        buf += (name_len + data_len);
        len -= (name_len + data_len);

        env_vars(impl.vars_)[name.c_str()] = data;
      }

      return ec;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::process_stdin(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf,std::size_t len
      , boost::system::error_code& ec)
    {
      if (0 == len)
      {
        impl.client_.status(common::stdin_read);
        return ec;
      }
      return ec;
    }

    /// Parse the current header
    template<typename Protocol>
    BOOST_CGI_INLINE boost::tribool
    fcgi_request_service<Protocol>::parse_header(implementation_type& impl)
    {
      BOOST_ASSERT(fcgi::spec::get_version(impl.header_buf_) == 1
                   && "This library is only compatible with FastCGI 1.0");

      using namespace fcgi::spec_detail;

      switch(fcgi::spec::get_type(impl.header_buf_))
      {
      case BEGIN_REQUEST:
      case DATA:
      case GET_VALUES:
        return boost::indeterminate;
      case STDIN:
        if (0 == fcgi::spec::get_length(impl.header_buf_)) {
          impl.client_.status(common::stdin_read);
          return true;
        } else
          return boost::indeterminate;
      case PARAMS:
        if (0 == fcgi::spec::get_length(impl.header_buf_)) {
          impl.client_.status(common::params_read);
          return true;
        } else
          return boost::indeterminate;
      case ABORT_REQUEST:
        return false;
      case UNKNOWN_TYPE:
      default:
        return true;
      }
    }

	template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::process_get_values(
        implementation_type& impl, boost::uint16_t id, boost::system::error_code& ec)
    {
      using namespace spec_detail;
      common::env_map vals;
      vals["FCGI_MAX_CONNS"] = "1";
      vals["FCGI_MPXS_CONNS"] = (impl.async_requests_ != 0) ? "1" : "0";
      vals["FCGI_MAX_REQS"] = boost::lexical_cast<std::string>(impl.async_requests_);

      std::vector<char> valuesbuff;
      std::for_each(std::begin(vals), std::end(vals), [&](std::pair<boost::cgi::common::name, std::string> const &pair)
      {
        std::string name_len = detail::param_length(pair.first);
        std::string value_len = detail::param_length(pair.second);

        std::copy(std::begin(name_len), std::end(name_len), std::back_inserter(valuesbuff));
        std::copy(std::begin(value_len), std::end(value_len), std::back_inserter(valuesbuff));
        std::copy(std::begin(pair.first), std::end(pair.first), std::back_inserter(valuesbuff));
        std::copy(std::begin(pair.second), std::end(pair.second), std::back_inserter(valuesbuff));
      });

      std::vector<char> buffer;
      Header hdr;
      hdr.reset(GET_VALUES_RESULT, id, valuesbuff.size());
      buffer.reserve(buffer.size() + sizeof(valuesbuff));
      char const* beg = reinterpret_cast<char const*>(&hdr);
      char const* end = beg + sizeof(hdr);
      std::copy(beg, end, std::back_inserter(buffer));
      std::copy(std::begin(valuesbuff), std::end(valuesbuff), std::back_inserter(buffer));

      std::vector<boost::asio::const_buffer> buffers;
      buffers.push_back(boost::asio::buffer(&(buffer.at(0)), buffer.size()));
      boost::asio::write(impl.client_.connection()->next_layer(), buffers, ec);
      return ec;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::parse_packet(
        implementation_type& impl, boost::system::error_code& ec)
    {
      if (this->read_header(impl, ec))
        return ec;

      boost::tribool state = this->parse_header(impl);

      if (state)
      { 
        // the header has been handled and all is ok.
        // **NOOP**
      } else
      if (!state)
      { // The header is confusing; something's wrong. Abort.
        ec = error::bad_header_type;
      }
      else
      {
        std::size_t remaining(fcgi::spec::get_length(impl.header_buf_));
        if (remaining)
        {
          mutable_buffers_type buf = impl.prepare(remaining);

          if (this->read_body(impl, buf, ec))
            return ec;

          this->parse_body(impl, buf, ec);
        }
      }
      return ec;
    }

    /// Read the body of the current packet; do nothing with it.
    template<typename Protocol>
    template<typename MutableBuffersType> BOOST_CGI_INLINE
    boost::system::error_code
    fcgi_request_service<Protocol>::read_body(
        implementation_type& impl, const MutableBuffersType& buf
      , boost::system::error_code& ec)
    {
      std::size_t bytes_read
        = read(*impl.client_.connection(), buf
              , boost::asio::transfer_all(), ec);

      BOOST_ASSERT(bytes_read == fcgi::spec::get_length(impl.header_buf_)
                   && "Couldn't read all of the record body.");
      return ec;
    }

	template<typename Protocol>
    template<typename MutableBuffersType> BOOST_CGI_INLINE
    boost::system::error_code
    fcgi_request_service<Protocol>::parse_body(
        implementation_type& impl, const MutableBuffersType& buf
      , boost::system::error_code& ec)
    {
      switch(fcgi::spec::get_type(impl.header_buf_))
      {
      case 1: process_begin_request(impl, fcgi::spec::get_request_id(impl.header_buf_)
              , boost::asio::buffer_cast<unsigned char*>(buf)
              , boost::asio::buffer_size(buf), ec);
              break;
      case 2: process_abort_request(impl, fcgi::spec::get_request_id(impl.header_buf_)
              , boost::asio::buffer_cast<unsigned char*>(buf)
              , boost::asio::buffer_size(buf), ec);
              break;
      case 4: process_params(impl, fcgi::spec::get_request_id(impl.header_buf_)
              , boost::asio::buffer_cast<unsigned char*>(buf)
              , boost::asio::buffer_size(buf), ec);
              break;
      case 5: process_stdin(impl, fcgi::spec::get_request_id(impl.header_buf_)
              , boost::asio::buffer_cast<unsigned char*>(buf)
              , boost::asio::buffer_size(buf), ec);
              break;
      default: break;
      }
      return ec;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::begin_request_helper(
        implementation_type& impl
      , header_buffer_type& header
      , boost::system::error_code& ec)
    {
       impl.client_.request_id_ = fcgi::spec::get_request_id(header);

       // A begin request body is as long as a header, so we can optimise:
       if (read_header(impl, ec))
         return ec;
        
       spec::begin_request packet(impl.header_buf_);
       impl.request_role_ = packet.role();
       impl.client_.role_ = static_cast<common::role_type>(packet.role());
       impl.client_.keep_connection_
         = packet.flags() & spec::keep_connection;

       impl.client_.status_ = common::constructed;
       
       return ec;
    }

 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/fcgi/request.hpp"
#include "boost/cgi/basic_request.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace fcgi {

	template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    fcgi_request_service<Protocol>::process_begin_request(
        implementation_type& impl, boost::uint16_t id
      , const unsigned char* buf, std::size_t len
      , boost::system::error_code& ec)
    {
      if (impl.client_.request_id_ == 0) // ie. hasn't been set yet.
      {
        begin_request_helper(impl, impl.header_buf_, ec);
      }
      else
      {
        //std::cerr<< "**FIXME** Role: " 
        //  << fcgi::spec::begin_request::get_role(impl.header_buf_) << std::endl;

        //implementation_type::client_type::connection_type&
        //  conn = *impl.client_.connection();
      }
      return ec;
    }

 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

#endif // CGI_FCGI_REQUEST_SERVICE_IPP_INCLUDED__

