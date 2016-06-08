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
// FastCGI errors are defined in here.
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_FCGI_ERROR_HPP_INCLUDED__
#define BOOST_FCGI_ERROR_HPP_INCLUDED__

#include <string>
#include <boost/system/error_code.hpp>

BOOST_CGI_NAMESPACE_BEGIN
 namespace fcgi {
  namespace error {

enum fcgi_errors
{
  bad_header_type = 1,

  /// A packet arrived for a request id that doesn't exist and the 
  /// packet wasn't a BEGIN_REQUEST record.
  bad_request_id,

  /// When trying to write a packet, the client::write_some() call didn't
  // transmit enough data before returning.
  couldnt_write_complete_packet,

  // Tried to read/write from/to the client associated to a request when it
  // was closed.
  client_closed,

  // Self-explanatory (not much a user can do about this though).
  abort_request_record_recieved_for_invalid_request,

  // For now a user has to recognise this error and construct a request
  // themselves. This is an ugly hack.
  multiplexed_request_incoming,

  // A begin_request packet has come in with an existing request id.
  duplicate_request,

  // Calling async_accept on a request that hasn't been closed isn't
  // allowed.
  accepting_on_an_open_request,

  invalid_socket,

  // On Windows, attempting to call DuplicateHandle on STDIN failed.
  unable_to_duplicate_handle,

  // On Windows, a call to SetStdHandle failed.
  failed_to_redirect_stdin,

  // On Windows, TCP connections aren't supported.
  unsupported_handle_type,

  // The CONTENT_TYPE for form data wasn't recognised.
  invalid_form_type,

  // Used in basic_connection<tags::stdio>
  // **FIXME**
  broken_pipe,

  // **FIXME**
  bad_read,

  // **FIXME**
  bad_write,

  // A client wasn't able to open.
  client_not_open,
  
  // Multiplexing connections are not yet supported.
  // (I have no access to a server that supports it)
  multiplexing_not_supported,
  
  // The client has already been closed.
  already_closed,
  
  // An empty FastCGI packet was read (eg. STDIN or GET_PARAM data has been read).
  //empty_packet_read,
  
  // End of File (read zero bytes)
  eof
};

  namespace detail {

class fcgi_category
  : public boost::system::error_category
{
public:
  const char* name() const BOOST_NOEXCEPT { return "fcgi_error"; }
  std::string message(int e) const
  {
    switch(e)
    {
    case client_closed:
      return "You are trying to read from or write to a closed client.";
    case multiplexed_request_incoming:
      return "A new request is pending on this connection (ie. it is "
             "multiplexed). This isn't handled for now. **FIXME**";
    case accepting_on_an_open_request:
      return "You called async_accept before closing a request.";
    case already_closed:
      return "The client has already been closed.";
    case multiplexing_not_supported:
      return "Multiplexing connections are not yet fully supported.";
    case unable_to_duplicate_handle:
      return "A call to DuplicateHandle failed while trying to duplicate the FastCGI HANDLE.";
    case failed_to_redirect_stdin:
      return "A call to SetStdHandle failed while trying to redirect the FastCGI HANDLE.";
    case unsupported_handle_type:
      return "An unsupported connection type was used to communicate with the FastCGI application.";
    //case empty_packet_read:
    //  return "An empty FastCGI packet was read (eg. STDIN or GET_PARAM data has been read).";
    default:
      return "An unknown FastCGI error occurred.";
    }
  }
};

  } // namespace detail

inline const boost::system::error_category& get_fcgi_category()
{
  static detail::fcgi_category instance;
  return instance;
}

static const boost::system::error_category& fcgi_category
  = ::BOOST_CGI_NAMESPACE::fcgi::error::get_fcgi_category();

  } // namespace error
 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

namespace boost {
  namespace system {

    template<> struct is_error_code_enum<
        ::BOOST_CGI_NAMESPACE::fcgi::error::fcgi_errors
      >
    {
      BOOST_STATIC_CONSTANT(bool, value = true);
    };

  } // namespace system
} // namespace boost

BOOST_CGI_NAMESPACE_BEGIN
 namespace fcgi {
  namespace error {
  
    inline boost::system::error_code make_error_code(fcgi_errors e)
    {
      return boost::system::error_code(
          static_cast<int>(e), get_fcgi_category());
    }

  } // namespace error
 } // namespace fcgi
BOOST_CGI_NAMESPACE_END

#endif // BOOST_FCGI_ERROR_HPP_INCLUDED__
