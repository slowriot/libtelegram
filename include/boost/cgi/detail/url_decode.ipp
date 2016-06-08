//                  -- url_decode.ipp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_URL_DECODE_IPP_INCLUDED__
#define CGI_DETAIL_URL_DECODE_IPP_INCLUDED__

#include "boost/cgi/detail/url_decode.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace detail {

   /// Convert two characters into a single, hex-encoded character
   BOOST_CGI_INLINE
   char hex_to_char(char const& c1, char const& c2)
   {
     int ret ( ( std::isalpha(c1)
                 ? ((c1 & 0xdf) - 'A') + 10
                 : (c1 - '0')
               ) << 4
             );

     ret += ( std::isalpha(c2)
              ? ((c2 & 0xdf) - 'A') + 10
              : (c2 - '0')
            );

     return static_cast<char>(ret);
   }

   /// Take two characters (a hex sequence) and return a char
   // **DEPRECATED** (use the above function)
   BOOST_CGI_INLINE
   char url_decode( const char& c1, const char& c2 )
   {
     int ret = ( (c1 >= 'A' && c1 <= 'Z') || (c1 >= 'a' && c1 <= 'z')
                   ? ((c1 & 0xdf) - 'A') + 10
                   : (c1 - '0')
                 ) << 4;

     ret += ( (c2 >= 'A' && c2 <= 'Z') || (c2 >= 'a' && c2 <= 'z')
                ? ((c2 & 0xdf) - 'A') + 10
                : (c2 - '0')
            );

     return static_cast<char>(ret);
   }

   /// URL-decode a string
   BOOST_CGI_INLINE
   std::string url_decode( const std::string& str )
   {
     typedef std::string string_type; // Ahem.
     string_type result;

     if (str.length() == 0)
    	 return str;

     for( string_type::const_iterator iter (str.begin()), end (str.end())
        ; iter != end; ++iter )
     {
       switch( *iter )
       {
         case ' ':
           break;
         case '+':
           result.append(1, ' ');
           break;
         case '%':
           if (std::distance(iter, end) <= 2
            || !std::isxdigit(*(iter+1))
            || !std::isxdigit(*(iter+2)))
           {
             result.append(1, '%');
           }
           else // we've got a properly encoded hex value.
           {
             char ch = *++iter; // need this because order of function arg
                                // evaluation is UB.
             result.append(1, hex_to_char(ch, *++iter));
           }
           break;
         default:
           result.append(1, *iter);
       }
     }

     return result;
   }

 } // namespace detail
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_DETAIL_URL_DECODE_IPP_INCLUDED__

