//                  -- url_decode.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_URL_DECODE_HPP_INCLUDED__
#define CGI_DETAIL_URL_DECODE_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

#include <string>

BOOST_CGI_NAMESPACE_BEGIN
 namespace detail {

   /// Convert a char into a hexadecimal value
   BOOST_CGI_INLINE std::string char_to_hex(char const& ch);

   /// Convert two characters into a single, hex-encoded character
   BOOST_CGI_INLINE char hex_to_char(char const& c1, char const& c2);

   /// Take two characters (a hex sequence) and return a char
   // **DEPRECATED**
   BOOST_CGI_INLINE char url_decode( const char& c1, const char& c2 );

   /// URL-decode a string
   BOOST_CGI_INLINE std::string url_decode( const std::string& str );

 } // namespace detail
BOOST_CGI_NAMESPACE_END

#if !defined( BOOST_CGI_BUILD_LIB )
#    include "boost/cgi/detail/url_decode.ipp"
#endif

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_DETAIL_URL_DECODE_HPP_INCLUDED__
