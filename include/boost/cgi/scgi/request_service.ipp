//             -- scgi/request_service.ipp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_SERVICE_IPP_INCLUDED__
#define CGI_SCGI_REQUEST_SERVICE_IPP_INCLUDED__

#include <boost/fusion/support.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/system/error_code.hpp>
#include <boost/fusion/include/vector.hpp>
////////////////////////////////////////////////////////////////
#include "boost/cgi/scgi/client.hpp"
#include "boost/cgi/scgi/error.hpp"
#include "boost/cgi/scgi/request_service.hpp"
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
#include "boost/cgi/scgi/request_impl.hpp"

BOOST_CGI_NAMESPACE_BEGIN

   namespace detail {
     
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

  namespace scgi {
 
    /// Close the request.
    template<typename Protocol>
    BOOST_CGI_INLINE int
    scgi_request_service<Protocol>::close(
        implementation_type& impl
      , common::http::status_code hsc
      , int program_status)
    {
      boost::system::error_code ec;
      close(impl, hsc, program_status, ec);
      detail::throw_error(ec);
      return program_status;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE int
    scgi_request_service<Protocol>::close(
        implementation_type& impl
      , common::http::status_code hsc
      , int program_status
      , boost::system::error_code& ec)
    {
      impl.all_done_ = true;
      impl.client_.close(program_status, ec);
      impl.request_status_ = common::closed;
      return program_status;
    }

	template<typename Protocol>
    BOOST_CGI_INLINE void
    scgi_request_service<Protocol>::clear(implementation_type& impl)
    {
      impl.post_buffer_.clear();
      // Clear all request data.
      boost::fusion::for_each(impl.vars_, detail::clear_data());
      impl.stdin_parsed_ = false;
      impl.http_status_ = common::http::no_content;
      impl.request_status_ = common::null;
      impl.all_done_ = false;
      impl.client_.status_ = common::none_;
      impl.client_.request_id_ = -1;
    }
      
    /// Load the request to a point where it can be usefully used.
    template<typename Protocol>
    BOOST_CGI_INLINE boost::system::error_code
    scgi_request_service<Protocol>::load(
        implementation_type& impl, common::parse_options parse_opts
      , boost::system::error_code& ec)
    {
      impl.client_.construct(impl, ec);

      // Bomb out if the client isn't open here.
      if (!impl.client_.connection()->is_open())
          return error::client_not_open;

      impl.buffer_.reset(new boost::array<char, 8192>);
      request_impl req;

      do
      {
        // Receive some more data. When control resumes at the following line,
        // the ec and length parameters reflect the result of the asynchronous
        // operation.
        std::size_t length
          = impl.client_.connection()->read_some(boost::asio::buffer(*impl.buffer_));

        // Parse the data we just received.
        boost::tie(impl.valid_request_, boost::tuples::ignore)
          = impl.request_parser_.parse(req,
              impl.buffer_->data(), impl.buffer_->data() + length);

        // An indeterminate result means we need more data, so keep looping.
      } while (boost::indeterminate(impl.valid_request_));

      if (!impl.valid_request_)
      {
        ec = error::invalid_request;
        return ec;
      }

      for(std::vector<header_impl>::iterator iter (req.headers.begin()), end (req.headers.end())
        ; iter != end; ++iter)
      {
        env_vars(impl.vars_)[iter->name.c_str()] = iter->value.c_str();
      }
      
      std::string const& cl = env_vars(impl.vars_)["CONTENT_LENGTH"];
      impl.bytes_left_
         = cl.empty() ? 0 : boost::lexical_cast<std::size_t>(cl);
      impl.client_.bytes_left()
         = impl.bytes_left_;
      std::string const& request_method
         = env_vars(impl.vars_)["REQUEST_METHOD"];

      if ((request_method == "GET" || request_method == "HEAD")
          && parse_opts > common::parse_env
          && parse_opts & common::parse_get_only)
      {
        this->parse_get_vars(impl, ec);
      }
      else
      if (request_method == "POST"
          && (parse_opts & common::parse_post_only))
      {
        parse_post_vars(impl, ec);
      }

      if (ec) return ec;

      if (parse_opts & common::parse_cookie_only)
      {
        if (this->parse_cookie_vars(impl, "HTTP_COOKIE", ec)) // returns an error_code
          return ec;
      }

      this->status(impl, common::loaded);

      return ec;
    }

    template<typename Protocol>
    template<typename Handler>
    BOOST_CGI_INLINE
    void scgi_request_service<Protocol>::do_load(
        implementation_type& impl, common::parse_options opts,
        Handler handler, boost::system::error_code& ec
      )
    {
      impl.client_.construct(impl, ec);
      // Bomb out if the client isn't open here.
      if (!impl.client_.connection()->is_open())
          ec = error::client_not_open;

    }

    // **FIXME**
    template<typename Protocol>
    template<typename Handler> BOOST_CGI_INLINE
    void scgi_request_service<Protocol>::async_load(
        implementation_type& impl, common::parse_options opts, Handler handler)
    {
      strand_.post(
        detail::async_load_helper<self_type, Handler>(this, opts, handler)
      );
    }

 } // namespace scgi
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/scgi/request.hpp"
#include "boost/cgi/scgi/request_parser.ipp"
#include "boost/cgi/basic_request.hpp"

#endif // CGI_SCGI_REQUEST_SERVICE_IPP_INCLUDED__
