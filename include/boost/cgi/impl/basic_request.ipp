//                -- basic_request.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Defines the basic_request<> class; the main entry-point to the
// library.
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_REQUEST_HPP_INCLUDED__
#define CGI_BASIC_REQUEST_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <boost/mpl/if.hpp>
#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/basic_io_object.hpp>
///////////////////////////////////////////////////////////
// **FIXME** Half of these are probably useless
#include "boost/cgi/detail/protocol_traits.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/is_async.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/common/status_type.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/common/request_service.hpp"
#include "boost/cgi/import/basic_io_object.hpp"
#include "boost/cgi/detail/basic_sync_io_object.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN

 namespace common {

  /// The basic_request class, primary entry point to the library
  /**
   * Note: By default, synchronous protocols (ie. cgi) auto-load AND parse
   * STDIN,whereas async protocols don't.
   *
   * Note: The alternative functions which take a boost::system::error_code are
   * the non-throwing versions. Instead of a boost::system::system_error being
   * thrown in case of an error, the passed error_code will be set to the value
   * of the error, s.t. if (error) evaluates to true.`
   * 
   * Note: This class isn't thread safe: carrying around a mutex-per-request
   * seems prohibitively expensive. There could be functions which take a mutex
   * as an arguement and lock it. (Async calls could get messy if you need a
   * protected request object).
  **/

    /// Make a new mutiplexed request from an existing connection.
    // Throws.
  template<typename RequestService, typename ProtocolService, typename Acceptor>
  basic_request<RequestService,ProtocolService,Acceptor>::    basic_request(implementation_type& impl)
      : basic_io_object<service_type>(impl.service_->io_service())
    {
      set_protocol_service(*impl.service_);
      boost::system::error_code ec;
      this->service.begin_request_helper(this->implementation
                                        , impl.header_buf_, ec);
      detail::throw_error(ec);
    }

    /// Make a new mutiplexed request from an existing connection.
    // Won't throw.
  template<typename RequestService, typename ProtocolService, typename Acceptor>
  basic_request<RequestService,ProtocolService,Acceptor>::    basic_request(implementation_type& impl, boost::system::error_code& ec)
      : basic_io_object<service_type>(impl.service_->io_service())
    {
      set_protocol_service(*impl.service_);
      this->service.begin_request_helper(this->implementation
                                        , impl.header_buf_, ec);
    }
      
    ~basic_request()
    {
      //if (is_open())
      //  close(http::internal_server_error, 0);
    }

  template<typename RequestService, typename ProtocolService, typename Acceptor>
  basic_request<RequestService,ProtocolService,Acceptor>::    static pointer create(protocol_service_type& ps)
    {
      return pointer(new type(ps));
    }

  template<typename RequestService, typename ProtocolService, typename Acceptor>
  basic_request<RequestService,ProtocolService,Acceptor>::    void set_protocol_service(protocol_service_type& ps)
    {
      this->service.set_service(this->implementation, ps);
    }

    /// Return `true` if the request is still open (ie. not aborted or closed)
  template<typename RequestService, typename ProtocolService, typename Acceptor>
  basic_request<RequestService,ProtocolService,Acceptor>::    bool is_open()
    {
      return this->service.is_open(this->implementation);
    }

    /// Synchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if parse_stdin == true
     */
    // Throwing semantics
    void load(bool parse_stdin = false)
    {
      boost::system::error_code ec;
      this->service.load(this->implementation, parse_stdin, ec);
      detail::throw_error(ec);
    }

    // Error-code semantics (**FIXME**)
    boost::system::error_code&
      load(bool parse_stdin, boost::system::error_code& ec)
    {
      return this->service.load(this->implementation, parse_stdin, ec);
    }

    // Error-code semantics (**FIXME**)
    boost::system::error_code&
      load(boost::system::error_code& ec, bool parse_stdin = false)
    {
      return this->service.load(this->implementation, parse_stdin, ec);
    }

    buffer_type& get_buffer()
    {
      return this->implementation.buffer_;
    }

    // **FIXME**
    /// Asynchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if parse_stdin == true
     */
    //template<typename Handler>
    //void async_load(Handler handler, bool parse_stdin = false)
    //{
    //  this->service.async_load(this->implementation, parse_stdin
    //                          , handler);
    //}

    /// Notify the server the request has finished being handled
    /**
     * In certain situations (such as a Proactor client using the async read
     * functions) it will be necessary to call end, rather than just returning
     * from the sub_main function.
     *
     * @param program_status This value is returned to the server indicating the
     * state of the request after it was finished handling. It is
     * implementation defined how the server deals with this, and it may have
     * no effect on the http status code returned to the client (eg. 200 OK).
     *
     * @returns The value of program_status
     */
    int close(common::http::status_code http_status = http::ok
             , int program_status = 0)
    {
      //BOOST_ASSERT( request_status_ != status_type::ended );

      //this->service.set_status(this->implementation, http_status);
      boost::system::error_code ec;
      this->service.close(this->implementation, http_status,
          program_status, ec);
      detail::throw_error(ec);
      return program_status;
    }

    int close(common::http::status_code http_status
             , int program_status
             , boost::system::error_code& ec)
    {
      return this->service.close(this->implementation, http_status
                                , program_status, ec);
    }

    /// Reject the request with a standard '500 Internal Server Error' error
    int reject()
    {
      this->service.set_status(this->implementation, aborted);
      return this->service.close(this->implementation
                                , http::internal_server_error);
    }

    /// Abort a request
    void abort()
    {
      this->service.set_status(this->implementation, aborted);
    }

    /// Clear the data for the request, for reusing this object.
    // I'd imagine clearing and re-loading a request is quicker than 
    // destroying/re-creating one. **Unverified claims** **FIXME**
    void clear()
    {
      this->service.clear(this->implementation);
    }

    /// Get the client connection associated with the request
    /**
     * You use the client for read/write calls. Y
		 */
    client_type& client()
    {
      return this->service.client(this->implementation);
    }

    /// Read some data into the request, parsing if necessary.
    void read_some()
    {
      boost::system::error_code ec;
      this->service.read_some(this->implementation, ec);
      detail::throw_error(ec);
    }

    /// Read some data into the request, parsing if necessary.
    std::size_t
    read_some(boost::system::error_code& ec)
    {
      return this->service.read_some(this->implementation, ec);
    }

    /// Read some data into the supplied buffer, parsing if necessary.
    // **FIXME** (remove - use req.client().read_some() instead)
    template<typename MutableBufferSequence>
    void read_some(const MutableBufferSequence& buf)
    {
      boost::system::error_code ec;
      this->service.read_some(this->implementation, buf, ec);
      detail::throw_error(ec);
    }

    /// Read some data into the supplied buffer, parsing if necessary.
    // **FIXME** (remove - use req.client().read_some() instead)
    template<typename MutableBufferSequence>
    std::size_t
    read_some(const MutableBufferSequence& buf
             , boost::system::error_code& ec)
    {
      return this->service.read_some(this->implementation, buf, ec);
    }

    /// Set the output for the request
    /**
     * Not Implemented Yet ******************
     *
     * Set the output sink as `stdout_`, `stderr_`, or `stdout_ | stderr_`
     */
    /*
    void set_output(BOOST_CGI_NAMESPACE::sink dest, boost::system::error_code& ec)
    {
      this->service(this->implementation, dest, ec);
    }
    */

    // [helper-functions for the basic CGI 1.1 meta-variables.
    string_type& auth_type()
    { return env_("AUTH_TYPE"); }

    string_type& content_length()
    { return env_("CONTENT_LENGTH"); }

    string_type& content_type()
    { return env_("CONTENT_TYPE"); }

    string_type& gateway_interface()
    { return env_("GATEWAY_INTERFACE"); }

    string_type& path_info()
    { return env_("PATH_INFO"); }

    string_type& path_translated()
    { return env_("PATH_TRANSLATED"); }

    string_type& query_string()
    { return env_("QUERY_STRING"); }

    string_type& remote_addr()
    { return env_("REMOTE_ADDR"); }

    string_type& remote_host()
    { return env_("REMOTE_HOST"); }

    string_type& remote_ident()
    { return env_("REMOTE_IDENT"); }

    string_type& remote_user()
    { return env_("REMOTE_USER"); }

    string_type& request_method()
    { return env_("REQUEST_METHOD"); }

    string_type& script_name()
    { return env_("SCRIPT_NAME"); }

    string_type& server_name()
    { return env_("SERVER_NAME"); }

    string_type& server_port()
    { return env_("SERVER_PORT"); }

    string_type& server_protocol()
    { return env_("SERVER_PROTOCOL"); }

    string_type& server_software()
    { return env_("SERVER_SOFTWARE"); }

    string_type& referer()
    { return env_("HTTP_REFERER"); }
    // -- end helper-functions]

    /// Get the charset from the CONTENT_TYPE header
    string_type charset()
    {
      // Not sure if regex is needlessly heavy-weight here.
      boost::regex re(";[ ]?charset=([-\\w]+);");
      boost::smatch match;
      if (!boost::regex_match(this->content_type(), match, re))
        return ""; // A default could go here.

      return match[1];
    }

    /// The role that the request is playing
    /**
     * The default role type is responder.
     *
     * In some cases - for instance with FastCGI - the role type can be
     * different
     * eg. authorizer, or filter.
     */
    role_type& role()
    {
      return this->service.get_role(this->implementation);
    }

    void set_status(common::http::status_code const& status)
    {
      this->service.set_status(this->implementation, status);
    }

    ////////////////////////////////////////////////////////////
    // Note on operator[]
    // ------------------
    // It is overloaded on different enum types to allow
    // compile-time (I hope) retrieval of different data
    // maps.
    //

    // The first three overloads are for directly looking into the 
    // environment.
    // eg.
    // string_type& val = req["some name"];

    /// Get the value of the environment variable with name `n`.
    string_type& operator[](string_type const& n)
    {
      return env_vars(this->implementation.vars_)[n.c_str()];
    }

    /// Get the value of the environment variable with name `n`.
    string_type& operator[](const char* n)
    {
      return env_vars(this->implementation.vars_)[n];
    }

    /// Get the value of the environment variable with name `n`.
    string_type& operator[](common::name const& n)
    {
      return env_vars(this->implementation.vars_)[n];
    }

    /// Get a `common::env_map&` of all the environment variables.
    env_map& operator[](common::env_data_type const&)
    {
      return env_vars(this->implementation.vars_);
    }

    /// Get a `common::get_map&` of all the GET variables.
    get_map& operator[](common::get_data_type const&)
    {
      return get_vars(this->implementation.vars_);
    }

    /// Get a `common::post_map&` of all the POST variables.
    post_map& operator[](common::post_data_type const&)
    {
      return post_vars(this->implementation.vars_);
    }

    /// Get a `common::cookie_map&` of all the cookies.
    cookie_map& operator[](common::cookie_data_type const&)
    {
      return cookie_vars(this->implementation.vars_);
    }

    /// Get a `common::form_map&` of either the GET or POST variables.
    form_map& operator[](common::form_data_type const&)
    {
      if (request_method() == "GET")
        return get_vars(this->implementation.vars_);
      else
      if (request_method() == "POST")
        return post_vars(this->implementation.vars_);
      else
        return env_vars(this->implementation.vars_);
    }
    ////////////////////////////////////////////////////////////

    /// The id of this request.
    /**
     * This is 1 for CGI/aCGI requests, but may be != 1 for FastCGI requests.
     * Note that for FastCGI requests, the id's are assigned on a
     * *per-connection* policy, so in one application you may have several
     * requests with the same id.
     */
    int id()
    {
      return this->service.request_id(this->implementation);
    }

  private:
    // Internal shortcut for named env-var functions (eg. script_name() above).
    string_type& env_(const char* name)
    {
      return env_vars(this->implementation.vars_)[name];
    }
  };

 } // namespace common
 
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_BASIC_REQUEST_HPP_INCLUDED__

/*
NOTES::future_plans:
* When a request is aborted (eg. when the client closes the connection)
  the library can call an abort_handler() function stored in the request.
  - The user should supply an abort_handler-derived function if they want
    any special handling of aborted requests
*/

