//                     -- error.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// Abstract:
// ---------
//
// The common errors are defined in here.
//
// **FIXME** This is a mess.
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_COMMON_ERROR_HPP_INCLUDED__
#define BOOST_COMMON_ERROR_HPP_INCLUDED__

#include <string>
#include <boost/system/error_code.hpp>
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {
  namespace error {

enum cgi_errors
{
  // Tried to read/write from/to the client associated to a request when it
  // was closed.
  client_closed = 1,

  // A begin_request packet has come in with an existing request id.
  duplicate_request,

  // Calling async_accept on a request that hasn't been closed isn't
  // allowed.
  accepting_on_an_open_request,

  invalid_socket,

  // The CONTENT_TYPE for form data wasn't recognised.
  invalid_form_type,

  // Used in basic_connection<tags::stdio>
  // **FIXME**
  broken_pipe,

  // **TODO** Document
  bad_read,

  // **TODO** Document
  bad_write,

  // A client wasn't able to open.
  client_not_open,
  
  // End of File (read zero bytes)
  eof,
  
  // The first multipart form boundary was not found.
  multipart_form_boundary_not_found,
  
  // The first multipart form boundary was not found.
  multipart_meta_data_not_terminated,
  
  // Expected a boundary marker for a multipart form, but did not find it.
  no_boundary_marker,
  
  already_closed,
  
  // The content length of the file upload is larger than maximum allowed
  // by the BOOST_CGI_POST_MAX macro.
  max_post_exceeded
};

  namespace detail {

class cgi_category
  : public boost::system::error_category
{
public:
  const char* name() const BOOST_NOEXCEPT { return "CGI Error"; }
  std::string message(int e) const
  {
    switch(e)
    {
    case client_closed:
      return "Attempting to read from or write to a client that has been"
             " closed.";
    case accepting_on_an_open_request:
      return "`async_accept` called with an open request (ie. it should be"
             " closed first).";
    case multipart_form_boundary_not_found:
      return "The first multipart form boundary was not found.";
    case no_boundary_marker:
      return "Expected a boundary marker for a multipart form, but did not"
             " find it.";
    case invalid_form_type:
      return "The CONTENT_TYPE for form data wasn't recognised.";
    case eof:
      return "End of File.";
    case duplicate_request:
      return "FastCGI: new request received with a duplicate id.";
    case max_post_exceeded:
      return "The content length of the file upload is larger than maximum"
             " allowed by the BOOST_CGI_POST_MAX macro.";
    case already_closed:
      return "Trying to close a request that has already been closed.";
    default:
      return "(CGI) BOOM!!!";
    }
  }
};

  } // namespace detail

inline const boost::system::error_category& get_cgi_category()
{
  static detail::cgi_category instance;
  return instance;
}

static const boost::system::error_category& cgi_category
  = ::BOOST_CGI_NAMESPACE::common::error::get_cgi_category();

  } // namespace error
 } // namespace common
BOOST_CGI_NAMESPACE_END

namespace boost {
  namespace system {

    template<> struct is_error_code_enum<
        ::BOOST_CGI_NAMESPACE::common::error::cgi_errors
      >
    {
      BOOST_STATIC_CONSTANT(bool, value = true);
    };

  } // namespace system
} // namespace boost

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {
  namespace error {
  
    inline boost::system::error_code make_error_code(cgi_errors e)
    {
      return boost::system::error_code(
          static_cast<int>(e), get_cgi_category());
    }

  } // namespace error
 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_COMMON_ERROR_HPP_INCLUDED__
