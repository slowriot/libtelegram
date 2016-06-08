//               -- cgi_service_impl.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__
#define CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/service_base.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/connections/async_stdio.hpp"

#include <boost/bind.hpp>
#include <boost/assert.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/find.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/common/is_async.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/common/form_parser.hpp"
#include "boost/cgi/common/request_base.hpp"
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/common/request_status.hpp"
#include "boost/cgi/connections/async_stdio.hpp"
#include "boost/cgi/detail/extract_params.hpp"
#include "boost/cgi/detail/save_environment.hpp"

BOOST_CGI_NAMESPACE_BEGIN

  class cgi_request_service
    : public common::request_base<common::tags::cgi>
    , public detail::service_base<cgi_request_service>
  {
  public:
    typedef common::tags::cgi                   protocol_type;
    typedef cgi_service                         protocol_service_type;
    typedef cgi_request_service                 self_type;

    struct implementation_type
      : base_type::impl_base
    {
      implementation_type()
        : stdin_data_read_(false)
        , stdin_bytes_left_(-1)
      {
      }

      protocol_service_type* service_;
        
      conn_ptr& connection()                   { return connection_;     }

      bool stdin_data_read_;
      std::size_t stdin_bytes_left_;
    
      conn_ptr connection_;
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
    

    cgi_request_service(common::io_service& ios)
      : detail::service_base<cgi_request_service>(ios)
    {
    }

    void construct(implementation_type& impl)
    {
      impl.client_.set_connection(
        connection_type::create(this->get_io_service())
      );
    }

#if BOOST_VERSION >= 104700
    void shutdown_service()
    {
    }
#endif

    void clear(implementation_type& impl) { }

    int request_id(implementation_type& impl) { return 1; }

    /// Close the request.
    int close(implementation_type& impl,
        common::http::status_code http_s = http::ok
      , int program_status = 0)
    {
      int s(0);
      boost::system::error_code ec;
      s = close(impl, http_s, program_status, ec);
      detail::throw_error(ec);
      return s;
    }

    /// Close the request.
    int close(implementation_type& impl,
        common::http::status_code http_s
      , int program_status, boost::system::error_code& ec)
    {
      status(impl, common::closed);
      impl.http_status() = http_s;
      impl.all_done_ = true;
      return program_status;
    }

    /// Synchronously read/parse the request data
    boost::system::error_code&
    load(implementation_type& impl, common::parse_options parse_opts
        , boost::system::error_code& ec)
    {
      if (parse_opts & common::parse_env)
      {
        if (read_env_vars(impl, ec)) // returns an error_code
          return ec;
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
        parse_get_vars(impl, ec);
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
        if (parse_cookie_vars(impl, "HTTP_COOKIE", ec)) // returns an error_code
          return ec;
      }

      status(impl, common::loaded);

      return ec;
    }

    /// CGI is always a responser.
    common::role_type
    role(implementation_type const& impl) const
    {
      return common::responder;
    }

    std::size_t
    read_some(implementation_type& impl, boost::system::error_code& ec)
    {
      return impl.client_.read_some(impl.prepare(64), ec);
    }

  protected:
    /// Read the environment variables into an internal map.
    template<typename RequestImpl>
    boost::system::error_code
    read_env_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // Only call this once.
      if (!(status(impl) & common::env_read))
      {
        detail::save_environment(env_vars(impl.vars_));
        status(impl, (common::request_status)(status(impl) | common::env_read));
      }
      return ec;
    }

    /// Read and parse the cgi POST meta variables (greedily)
    template<typename RequestImpl>
    boost::system::error_code
    parse_post_vars(RequestImpl& impl, boost::system::error_code& ec)
    {
      // **FIXME** use callback_functor<> in form_parser instead.
      std::size_t& bytes_left (impl.client_.bytes_left_);
      std::size_t bytes_read (0);
      do {
         bytes_read = read_some(impl, ec);
         bytes_left -= bytes_read;
      } while (!ec && bytes_left);
      
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
  };

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_CGI_SERVICE_IMPL_HPP_INCLUDED__
