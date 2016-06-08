//                -- basic_request.hpp --
//
//          Copyright (c) 2007-2009 Darren Garvey.
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

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/request_data.hpp"
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/common/path_info.hpp"
#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/common/request_service.hpp"
#include "boost/cgi/common/request_status.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/detail/basic_io_object.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"
#include "boost/cgi/http/status_code.hpp"
#ifdef BOOST_CGI_ENABLE_SESSIONS
#  include "boost/cgi/utility/sessions.hpp"
#endif // BOOST_CGI_ENABLE_SESSIONS

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// The basic_request class, primary entry point to the library
  /**
   * Note: By default, synchronous protocols (ie. cgi) auto-load AND parse
   * STDIN,whereas async protocols don't.
   *
   * Note: The alternative functions which take a boost::system::error_code
   * are the non-throwing versions. Instead of a boost::system::system_error
   * being thrown in case of an error, the passed error_code will be set to
   * the value of the error, s.t. if (error) evaluates to true.`
   * 
   * Note: This class isn't thread safe: carrying around a mutex-per-request
   * seems prohibitively expensive. There could be functions which take a
   * mutex as an argument and lock it. (Async calls could get messy if you
   * need a protected request object).
  **/
  template<typename Protocol>
  class basic_request
    : public detail::basic_io_object<
        typename protocol_traits<Protocol>::request_service_type
    >
  {
  public:
    typedef basic_request<Protocol>                    self_type;
    typedef Protocol                                   protocol_type;
    typedef protocol_traits<protocol_type>             traits;
    typedef typename traits::protocol_service_type     protocol_service_type;
    typedef typename traits::request_service_type      service_type;
    typedef typename service_type::implementation_type implementation_type;
    typedef typename traits::char_type                 char_type;
    typedef typename traits::string_type               string_type;
    typedef typename traits::request_type              request_type;
    typedef typename traits::client_type               client_type;
    typedef typename traits::buffer_type               buffer_type;
#ifdef BOOST_CGI_ENABLE_SESSIONS
    typedef typename traits::session_type              session_type;

    session_type session;
#endif // BOOST_CGI_ENABLE_SESSIONS
    typedef boost::shared_ptr<request_type>            pointer;
    
    /// The environment data, exposed as a `request_data<env_map>`.
    env_data    env;
    /// The GET (ie. query string) data, exposed as a `request_data<get_map>`.
    post_data   post;
    /// The POST data, exposed as a `request_data<post_map>`.
    get_data    get;
    /// The form data, which is either the GET or POST data.
    form_data   form;
    /// The cookie data, exposed as a `request_data<cookie_map>`.
    cookie_data cookies;
    /// The file uploads, exposed as a `request_data<upload_map>`.
    upload_data uploads;

    basic_request(
        int opts
      , char** base_env = NULL)
        : detail::basic_io_object<service_type>()
    {
      if ((parse_options)opts > parse_none) load((parse_options)opts, base_env);
    }

     basic_request(
        const parse_options opts = traits::parse_opts
      , char** base_env = NULL)
        : detail::basic_io_object<service_type>()
    {
      set_protocol_service();
      if (opts > parse_none) load(opts, base_env);
    }

    // Won't throw
    basic_request(boost::system::error_code& ec
                 , const parse_options opts = traits::parse_opts
                 , char** base_env = NULL)
      : detail::basic_io_object<service_type>()
    {
      set_protocol_service();
      if (opts > parse_none) load(opts, ec);
    }

    // Throws
    basic_request(protocol_service_type& s
                 , const parse_options opts = traits::parse_opts
                 , char** base_env = NULL)
      : detail::basic_io_object<service_type>(s.get_io_service())
    {
      set_protocol_service(s);
      if (opts > parse_none) load(opts, base_env);
    }

    // Won't throw
    basic_request(protocol_service_type& s
                 , boost::system::error_code& ec
                 , const parse_options opts = traits::parse_opts
                 , char** base_env = NULL)
      : detail::basic_io_object<service_type>(s.get_io_service())
    {
      set_protocol_service(s);
      if (opts > parse_none) load(opts, ec, base_env);
    }

    /// Make a new mutiplexed request from an existing connection.
    // Throws.
    basic_request(implementation_type& impl)
      : detail::basic_io_object<service_type>(impl.service_->get_io_service())
    {
      set_protocol_service(*impl.service_);
      boost::system::error_code ec;
      this->service.begin_request_helper(this->implementation
                                        , impl.header_buf_, ec);
      detail::throw_error(ec);
    }

    /// Make a new mutiplexed request from an existing connection.
    // Won't throw.
    basic_request(implementation_type& impl, boost::system::error_code& ec)
      : detail::basic_io_object<service_type>(impl.service_->get_io_service())
    {
      set_protocol_service(*impl.service_);
      this->service.begin_request_helper(this->implementation
                                        , impl.header_buf_, ec);
    }
      
    ~basic_request()
    {
#ifdef BOOST_CGI_ENABLE_SESSIONS
      try {
          if (!session.id().empty())
            this->service.session_manager().save(session);
      } catch(...) {
         // pass
      }
#endif // BOOST_CGI_ENABLE_SESSIONS
    }
    
#ifdef BOOST_CGI_ENABLE_SESSIONS
    /// Start a session, or load the session from a cookie.
    /**
     * This starts a session.
	 */
    void start_session()
    {
      if (!session.loaded())
      {
        // Assume cookies have been loaded. This will throw at runtime (in
        // a debug build) if `request.load(parse_cookie)` hasn't been called
        // by now.
        this->service.session_manager().start(
            session, cookies.pick(BOOST_CGI_SESSION_COOKIE_NAME, ""));
      }
    }

    /// Stop the session and delete the stored session data.
    void stop_session()
    {
      this->service.session_manager().stop(session);
    }
#endif // BOOST_CGI_ENABLE_SESSIONS

    protocol_service_type& get_protocol_service()
    {
      return *(this->implementation.service_);
    }
    
    static pointer create(protocol_service_type& ps)
    {
      return pointer(new self_type(ps));
    }

    static pointer create()
    {
      return pointer(new self_type());
    }

    void set_protocol_service()
    {
      //this->service.set_service(this->implementation, ps);
    }

    void set_protocol_service(protocol_service_type& ps)
    {
      this->service.set_service(this->implementation, ps);
    }

    /// The id of this request.
    /**
     * This is 1 for CGI/aCGI requests, but may be != 1 for FastCGI
     * requests.
     *
     * Note that for FastCGI requests, the id's are assigned on a
     * *per-connection* policy, so in one application you may have 
     * several requests with the same id.
     */
    int id()
    {
      return this->service.request_id(this->implementation);
    }

    /// Check if the request is still open (ie. not aborted or closed)
    bool is_open()
    {
      return this->service.is_open(this->implementation);
    }

    /// Synchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if
     * parse_stdin == true
     */
    void load(parse_options parse_opts = parse_env, char** base_env = NULL)
    {
      boost::system::error_code ec;
      load(parse_opts, ec, base_env);
      if (ec != error::eof)
        detail::throw_error(ec);
    }

    // Error-code semantics
    boost::system::error_code
      load(parse_options parse_opts, boost::system::error_code& ec
          , char** base_environment = NULL, bool is_command_line = true)
    {
      // Parse just the environment first, then check the user
      // isn't trying to upload more data than we want to let them.
      // Define `BOOST_CGI_POST_MAX` to set the maximum content-length
      // allowed.
      if (parse_opts & parse_env)
      {
        //this->service.load(this->implementation, parse_env, ec);
        //if (content_length() >= BOOST_CGI_POST_MAX)
        //  ec = common::error::max_post_exceeded;
        this->service.load(this->implementation, parse_opts, ec);
        
        if (ec) return ec;

        // Load the environment passed by the user.
        if (base_environment)
          this->service.load_environment(
              this->implementation, base_environment
            , is_command_line);
        
        if (parse_opts & parse_env)
          env.set(env_vars(this->implementation.vars_));
        if (parse_opts & parse_get_only)
          get.set(get_vars(this->implementation.vars_));
        if (parse_opts & parse_post_only) {
          post.set(post_vars(this->implementation.vars_));
          uploads.set(upload_vars(this->implementation.vars_));
        }
        if ((parse_opts & parse_cookies) == parse_cookies) {
          cookies.set(cookie_vars(this->implementation.vars_));
        }
        if ((parse_opts & parse_form_only) == parse_form_only)
        {
          common::name rm(request_method().c_str());
          form.set(rm == "POST" ? post.impl() : get.impl());
        }
#ifdef BOOST_CGI_ENABLE_SESSIONS
        if (parse_opts & parse_session_only)
        {
          if (!!cookies && cookies.count(BOOST_CGI_SESSION_COOKIE_NAME))
            session.id(cookies[BOOST_CGI_SESSION_COOKIE_NAME]);
          else
          if (traits::auto_start_session)
            session.id(this->service.make_session_id());
          if (!session.id().empty())
            this->service.session_manager().load(session);
        }
#endif // BOOST_CGI_ENABLE_SESSIONS
      }
      return ec;
    }

    void load(char** base_environment, bool is_command_line = true)
    {
      this->service.load_environment(this->implementation
                                    , base_environment
                                    , is_command_line);
    }

    /// Get the buffer containing the POST data.
    /**
     * **FIXME**
     * This actually returns the whole buffer on FastCGI at the moment, 
     * which contains the params too.
     */
    buffer_type& post_buffer()
    {
      return this->implementation.post_buffer_;
    }
    
    // **FIXME**
    /// Asynchronously read/parse the request meta-data
    /**
     * Note: 'loading' including reading/parsing STDIN if
     * parse_stdin == true
     */
    //template<typename Handler>
    //void async_load(Handler handler, bool parse_stdin = false)
    //{
    //  this->service.async_load(this->implementation, parse_stdin
    //                          , handler);
    //}

    /// Notify the server the request has been handled.
    /**
     * In certain situations (such as a Proactor client using the async
     * read functions) it will be necessary to call end, rather than 
     * just returning from the sub_main function.
     *
     * @param http_status The HTTP status of the request.
     *
     * @param program_status This value is returned to the server
     * indicating the state of the request after it was finished 
     * handling. It is implementation defined how the server deals with
     * this, and it may have no effect on the http status code returned
     * to the client (eg. 200 OK).
     *
     * @returns The value of program_status
     */
    int close(common::http::status_code http_status = http::ok
             , int program_status = 0)
    {
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

    /// Reject the request with a '500 Internal Server Error' error.
    int reject()
    {
      this->service.status(this->implementation, aborted);
      return this->service.close(this->implementation
                                , http::internal_server_error);
    }

    /// Abort a request.
    void abort()
    {
      this->service.status(this->implementation, aborted);
    }

    /// Clear the data for the request, for reusing this object.
    void clear()
    {
      this->service.clear(this->implementation);
    }

    /// Get the client connection associated with the request
    /**
     * You use the client for read/write calls.
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

    /////////////////////////////////////////////////////////
    // Helper-functions for the basic CGI 1.1 meta-variables.
    /////////////////////////////////////////////////////////
    
    string_type& auth_type()
    { return env["AUTH_TYPE"]; }

    /// Get the content length as a long.
    /**
     * The content length defaults to zero if it isn't explicitly set
     * by your HTTP server.
     */
    long content_length()
    { 
      string_type& cl(env["CONTENT_LENGTH"]);
      return boost::lexical_cast<long>(cl.empty() ? "0" : cl); 
    }

    /// The content type of the request.
    /**
     * Common value: text/html.
     */
    string_type& content_type()
    { return env["CONTENT_TYPE"]; }

    /// The protocol used by the server to communicate to the script.
    /**
     * Common value: CGI/1.1.
     */
    string_type& gateway_interface()
    { return env["GATEWAY_INTERFACE"]; }

    /// Additional information, appendended to the script.
    common::path_info path_info()
    { return env["PATH_INFO"]; }

    /// The translated version of the path info.
    /**
     * Your HTTP server may provide this, depending on configuration. 
     * The path info can represent a resource on the local file system.
     */
    string_type& path_translated()
    { return env["PATH_TRANSLATED"]; }

    /// The query string for the request.
    /**
     * This is the part of the request URI after a '?'. A GET request
     * passes request parameters, URL encoded in the query string.
     */
    string_type& query_string()
    { return env["QUERY_STRING"]; }

    /// The host address of the remote user.
    string_type& remote_addr()
    { return env["REMOTE_ADDR"]; }

    /// The host name of the remote user's machine.
    string_type& remote_host()
    { return env["REMOTE_HOST"]; }

    /// The user making the request.
    string_type& remote_ident()
    { return env["REMOTE_IDENT"]; }

    /// The userid of the person accessing the script.
    string_type& remote_user()
    { return env["REMOTE_USER"]; }

    /// The method of the request.
    /**
     * Common values: `GET`, `POST`, `HEAD`.
     */
    string_type& method()
    { return env["REQUEST_METHOD"]; }

    /// The method of the request (long-hand of `method()`).
    /**
     * Common values: `GET`, `POST`, `HEAD`.
     */
    string_type& request_method()
    { return env["REQUEST_METHOD"]; }

    /// Get the URI of the request.
    string_type& uri()
    { return env["REQUEST_URI"]; }

    /// Get the URI of the request (long-hand of `uri()`).
    string_type& request_uri()
    { return env["REQUEST_URI"]; }

    /// The name of the script.
    string_type& script_name()
    { return env["SCRIPT_NAME"]; }

    /// The URL of the script.
    string_type& script_url()
    { return env["SCRIPT_URL"]; }

    /// The full URI of the script.
    string_type& script_uri()
    { return env["SCRIPT_URI"]; }

    /// Get the name of the server.
    /**
     * Usually set in your HTTP configuration. This could be the name
     * of a virtual host.
     */
    string_type& server_name()
    { return env["SERVER_NAME"]; }

    /// Get the port the calling server is listening on.
    /**
     * Common value: 80.
     */
    string_type& server_port()
    { return env["SERVER_PORT"]; }

    /// Get the protocol being used by the calling server.
    /**
     * Common value: HTTP/1.1.
     */
    string_type& server_protocol()
    { return env["SERVER_PROTOCOL"]; }

    /// Get a string identifying the calling server.
    /**
     * CGI scripts are generally called by a web-facing HTTP server.
     * When set, this string can be useful for knowing what is calling 
     * the script, especially in a multi-server or load balanced
     * environment.
     */
    string_type& server_software()
    { return env["SERVER_SOFTWARE"]; }
    
    /// Get the web page the user came from.
    /**
     * HTTP equivalent: `HTTP_REFERER`
     *
     * The referer is commonly used for tracking user's movements and
     * origins. For instance, some sites use this to highlight some
     * keywords on the page when users come from a search engine.
     *
     * Note that you cannot ever guarantee on this value being either set
     * or accurate.
     */
    string_type& referer()
    { return env["HTTP_REFERER"]; }
    // -- end helper-functions]

    /// Get the charset from the CONTENT_TYPE header
    string_type charset()
    {
      string_type ctype(content_type());
      std::size_t pos = ctype.find("charset=");
      string_type val(ctype.substr(pos+8, ctype.find(";", pos)));
      boost::algorithm::trim(val);
      return val.empty() ?  BOOST_CGI_DEFAULT_CHARSET : val;
    }
    
    /// The email of the user making the request.
    string_type& http_from ()
    { return env["HTTP_FROM"]; }

    /// The cookies sent by the user making the request.
    string_type& http_cookie ()
    { return env["HTTP_COOKIE"]; }

    /// The role that the request is playing
    /**
     * The default role type is responder.
     *
     * In some cases - for instance with FastCGI - the role type can be
     * different
     * eg. `authorizer`, or `filter`.
     */
    role_type role() const
    {
      return this->service.role(this->implementation);
    }

    /// Get / Set the status of a request.
    /**
     * The usual way to set the request status is to set it
     * when calling `close`.
     */
    common::request_status status() const
    {
      return this->service.status(this->implementation);
    }
    
    /// Set the status of a request.
    void status(common::request_status const& status)
    {
      this->service.status(this->implementation, status);
    }

    /// Sets the maximum number of async requests.
    boost::uint16_t& async_requests()
    {
      return this->implementation.async_requests_;
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
