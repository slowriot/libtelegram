//                    -- config.hpp --
//
//          Copyright (c) Darren Garvey 2007-2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_CONFIG_HPP_INCLUDED__
#define BOOST_CGI_CONFIG_HPP_INCLUDED__

namespace boost {
 namespace cgi {}
} // namespace boost

/// Make the library keep file uploads on disk.
/**
 * File uploads are save to the hard disk in an upload directory. By default,
 * they are deleted at the end of the request.
 */
//#define BOOST_CGI_KEEP_FILE_UPLOADS

/// Prevent the library from using boost filesystem
/**
 * Sessions and uploads use boost::filesystem, which creates a linker 
 * dependency.  Sometimes this is not desirable, i.e. when not using sessions
 * or file uploads.
 */
//#define BOOST_CGI_NO_BOOST_FILESYSTEM

#ifndef NDEBUG
# include <iostream>
#endif // NDEBUG

/// Characters that should be removed from any file uploads.
/**
 * The filename of any file uploads is a stripped version of
 * the provided file's name. As such, we need to clean the uploaded
 * file's name.
 */
#ifndef BOOST_CGI_UNSAFE_FILENAME_CHARS
#   define BOOST_CGI_UNSAFE_FILENAME_CHARS ":~.|/\\"
#endif // BOOST_CGI_UNSAFE_FILENAME_CHARS

/// The maximum amount of POST data that can be uploaded.
/**
 * Set this to -1 for no limit.
 */
#ifndef BOOST_CGI_POST_MAX
    /// Restrict POST data to less than 7MB per request.
#   define BOOST_CGI_POST_MAX 6663322
#endif // BOOST_CGI_POST_MAX

/// The default charset to add to responses.
#ifndef BOOST_CGI_DEFAULT_CHARSET
#   define BOOST_CGI_DEFAULT_CHARSET "UTF-8"
#endif

/// The namespace of the library.
/**
 * Default value:
 *   boost::cgi
 * which puts the library in the boost::cgi namespace.
 */
#ifndef BOOST_CGI_NAMESPACE
# define BOOST_CGI_NAMESPACE boost::cgi
#endif // BOOST_CGI_NAMESPACE

/// The opening namespace tag.
/**
 * Default value:
 *   namespace boost { namespace cgi {
 * which puts the library in the boost::cgi namespace.
 */
#ifndef BOOST_CGI_NAMESPACE_BEGIN
# define BOOST_CGI_NAMESPACE_BEGIN namespace boost { namespace cgi {
#endif // BOOST_CGI_NAMESPACE_BEGIN

/// The closing namespace tag.
/**
 * Default value:
 *   } } // namespace BOOST_CGI_NAMESPACE
 * which ends the boost::cgi namespaces.
 */
#ifndef BOOST_CGI_NAMESPACE_END
# define BOOST_CGI_NAMESPACE_END } } // namespace BOOST_CGI_NAMESPACE
#endif // BOOST_CGI_NAMESPACE_END

/// Library assertions, only in debug mode.
#ifndef BOOST_CGI_DEBUG_ASSERT
# ifdef NDEBUG
#   define BOOST_CGI_DEBUG_ASSERT(x) /** void **/
# else
#   define BOOST_CGI_DEBUG_ASSERT(x) assert(x)
# endif // NDEBUG
#endif // BOOST_CGI_DEBUG_ASSERT

/// Library assertions.
#ifndef BOOST_CGI_ASSERT
# ifdef NDEBUG
#   define BOOST_CGI_ASSERT(x) BOOST_ASSERT(x)
# else
#   define BOOST_CGI_ASSERT(x) BOOST_CGI_DEBUG_ASSERT(x)
# endif // NDEBUG
#endif // BOOST_CGI_ASSERT

/// Shortcut macro for defining custom protocols based on existing ones.
#ifndef BOOST_CGI_OVERRIDE_PROTOCOL
# define BOOST_CGI_OVERRIDE_PROTOCOL(protocol, tag, brace_enclosed_traits) \
    struct tag##;\
    namespace boost { namespace cgi { namespace common {\
      template<>\
      struct protocol_traits<tag>\
        : protocol_traits<tags::##protocol>\
        brace_enclosed_traits\
      ; } } } \
    struct tag##{\
      typedef boost::cgi::common::basic_request_acceptor<tag> acceptor;\
      typedef boost::cgi::common::basic_request<tag>          request;\
      typedef boost::cgi::common::basic_protocol_service<tag> service;\
    };
#endif // BOOST_CGI_OVERRIDE_PROTOCOL

#endif // BOOST_CGI_CONFIG_HPP_INCLUDED__
