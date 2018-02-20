//                   -- response.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_RESPONSE_HPP_INCLUDED__
#define CGI_RESPONSE_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <string>
///////////////////////////////////////////////////////////
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/cookie.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/import/streambuf.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// The response class: a helper for responding to requests.
  /**
   * This class has no knowledge of requests or protocols. It is a lightweight
   * wrapper over a std::ostream that separates response headers from the
   * response body.
   *
   * One response can be reused for several requests (though not
   * simultaneously) and multiple responses can be combined.
   *
   * Note that std::endl is not supported for this class. std::endl usually
   * flushes an std::ostream, which is not possible with this class as it
   * is request-agnostic.
   *
   * When a response has been constructed, you can send it to any
   * SyncWriteStream by calling send() or flush().
   */
  template<typename CharT>
  class basic_response
  {
  public:
    typedef basic_response<CharT>              self_type;
    typedef CharT                              char_type;
    typedef typename std::basic_string<CharT>  string_type;
    typedef typename std::basic_ostream<CharT> ostream_type;

    basic_response(common::http::status_code sc = common::http::ok);

    /// Construct with a particular buffer
    /**
     * Takes the buffer and uses it internally, does nothing with it on
     * destruction.
     */
    basic_response(::BOOST_CGI_NAMESPACE::common::streambuf* buf,
        common::http::status_code sc = common::http::ok);

    ~basic_response();

    /// Clear the response buffer.
    void clear(bool clear_headers = true);

    /// Return the response to the 'just constructed' state.
    void reset();

    /// Write characters to the response body.
    std::size_t write(const char_type* str, std::size_t len);

    /// Write a string to the response body.
    std::size_t write(string_type const& str);

    template<typename ConstBufferSequence>
    std::size_t write(const ConstBufferSequence& buf);

    /// Synchronously flush the data to the supplied SyncWriteStream.
    /**
     * This call uses throwing semantics. ie. an exception will be thrown on
     * any failure.
     * If there is no error, the buffer is cleared.
     */
    template<typename SyncWriteStream>
    void flush(SyncWriteStream& sws);
    
    /// Put a character into the stream.
    self_type& put (char_type c);

    /// Synchronously flush the data via the supplied SyncWriteStream.
    /**
     * This call uses error_code semantics. ie. ec is set if an error occurs.
     * If there is no error, the buffer is cleared.
     */
    template<typename SyncWriteStream>
    boost::system::error_code
      flush(SyncWriteStream& sws, boost::system::error_code& ec);

    /// Synchronously send the data via the supplied request.
    /**
     * This call uses throwing semantics. ie. an exception will be thrown on
     * any failure.
     * Note: The data in the stream isn't cleared after this call.
     */
    template<typename SyncWriteStream>
    void send(SyncWriteStream& sws);

    /// Synchronously send the data via the supplied request.
    /**
     * This call will not throw, but will set `ec` such that `ec == true` if
     * an error occurs. Details of the error are held in the `error_code`
     * object.
     */
    template<typename SyncWriteStream>
    boost::system::error_code
      send(SyncWriteStream& sws, boost::system::error_code& ec);

    /// Resend headers + content regardless of value of `headers_terminated_`.
    template<typename SyncWriteStream>
    void resend(SyncWriteStream& sws);

    /// Asynchronously send the data through the supplied AsyncWriteStream.
    /**
     * Note: This is quite crude at the moment and not as asynchronous as
     *       it could/should be. The data in the stream isn't cleared after
     *       this call.
     */
    template<typename AsyncWriteStream, typename Handler>
    void async_send(AsyncWriteStream& aws, Handler handler);

    template<typename AsyncWriteStream, typename Handler>
    void do_async_send(AsyncWriteStream& aws, Handler handler);

    /// Get the buffer associated with the stream
    common::streambuf*
      rdbuf();

    /// Set the status code associated with the response.
    basic_response<char_type>&
      status(const http::status_code& num);

    /// Get the status code associated with the response.
    http::status_code status() const;

    /// Allow more headers to be added (WARNING: avoid using this).
    void unterminate_headers();

    /// Get the length of the body of the response (ie. not including the headers).
    std::size_t content_length();

    /// Add a header after appending the CRLF sequence.
    basic_response<char_type>&
      set(basic_header<char_type> const& hdr)
    {
      if (hdr.content.empty())
        end_headers();
      else
        set_header(hdr.content);
      return *this;
    }

    /// Add a cookie to the response.
    basic_response<char_type>&
      set(const basic_cookie<char_type>& ck)
    {
      set_header("Set-Cookie", ck.to_string());
      return *this;
    }

    /// Set a header give a string of the form "name=value".
    basic_response<char_type>&
      set_header(const string_type& value);

    /// Format and add a header given name and value, appending CRLF.
    basic_response<char_type>&
      set_header(string_type const& name, string_type const& value);

    /// Get the contents of the response as a string.
    /**
     * This copies the contents of the response into a string.
     * Headers aren't included unless `include_header` is true.
     */
    string_type str(bool include_header = false) const;

    /// Get the value of a header in this response with the name `name`.
    string_type header_value(string_type const& name);

    /// Returns true for existing header names.
    bool has_header_value(string_type const& name);


    /// Clear all of the response headers.
    void clear_headers();

    void reset_headers();
    
    /// Get the charset.
    string_type& charset() const { return charset_; }

    /// Set the charset.
    void charset(string_type const& cs) { charset_ = cs; }

    /// Check if more response headers may be added.
    bool headers_terminated() const;

    // Is this really necessary?
    void end_headers();

    /// Get the ostream containing the response body.
    ostream_type& ostream();

    /// Get the response headers.
    std::vector<string_type>& headers();

    /// Stream any type to the response.
    template<typename T>
    self_type& operator<<(T t)
    {
      ostream_<< t;
      return *this;
    }

    /// Change the charset of the response.
    self_type& operator<< (charset_header<char_type> const& hdr)
    {
      charset(hdr.content);
      return *this;
    }
 
    /// Add a header to the response.
    self_type& operator<< (basic_header<char_type> const& hdr)
    {
      return set(hdr);
    }
 
    /// Add a cookie to the response.
    self_type& operator<< (basic_cookie<char_type> const& ck)
    {
      return set(ck);
    }
 
    /// Set the HTTP status of the response.
    self_type& operator<< (http::status_code stat)
    {
      status(stat);
      return *this;
    }

    /// Copy another response into this response.
    /**
     * The body of the other response is appended to the body of this
     * response. The headers of the other response are added to this
     * response, except for the "Content-type" header. The content-type
     * of this response is assumed to be authoritative.
     */
    self_type& operator<< (self_type& other)
    {
      if (!headers_terminated())
      {
        typedef std::vector<std::string>::const_iterator iter_t;
        for(iter_t iter (other.headers().begin()), end (other.headers().end());
            iter != end;
            ++iter
          )
        {
          if (iter->substr(0,13) != "Content-type:") // Don't overwrite the content-type.
            headers_.push_back(*iter);
        }
      }
      ostream_<< other.ostream().rdbuf();
      return *this;
    }
 
  protected:
    // Vector of all the headers, each followed by a CRLF
    std::vector<string_type> headers_;

    // The buffer is a shared_ptr, so you can keep it cached elsewhere.
    boost::shared_ptr<common::streambuf> buffer_;

    ostream_type ostream_;

    http::status_code http_status_;

    // True if no more headers can be appended.
    bool headers_terminated_;

    string_type charset_;

 private:
    // Send the response headers and mark that they've been sent.
    template<typename ConstBufferSequence>
    void prepare_headers(ConstBufferSequence& headers);
   };

   /// Typedefs for typical usage.
   typedef basic_response<char>    response;

 } // namespace common
BOOST_CGI_NAMESPACE_END



  /// Generic ostream template
  /*
  template<typename CharT, typename T>
  BOOST_CGI_NAMESPACE::common::basic_response<CharT>&
    operator<< (BOOST_CGI_NAMESPACE::common::basic_response<CharT>& resp, T t);
  */
  /// You can stream a BOOST_CGI_NAMESPACE::cookie into a response.
  /**
   * This is just a shorthand way of setting a header that will set a
   * client-side cookie.
   *
   * You cannot stream a cookie to a response after the headers have been
   * terminated. In this case, an alternative could be to use the HTML tag:
   * <meta http-equiv="Set-cookie" ...> (see http://tinyurl.com/3bxftv or
   * http://tinyurl.com/33znkj), but this is outside the scope of this
   * library.
   */
              
  /*
   template<typename charT>
  BOOST_CGI_NAMESPACE::common::basic_response<charT>&
    operator<< (BOOST_CGI_NAMESPACE::common::basic_response<charT>&
               , BOOST_CGI_NAMESPACE::common::basic_cookie<charT>);
  */             
#include "boost/cgi/detail/pop_options.hpp"

#if !defined(BOOST_CGI_BUILD_LIB)
#  include "boost/cgi/impl/response.ipp"
#endif

#endif // CGI_RESPONSE_HPP_INCLUDED__
