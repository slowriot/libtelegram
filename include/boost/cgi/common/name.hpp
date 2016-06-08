//                 -- common/name.hpp --
//
//           Copyright (c) Darren Garvey 2008.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
//
// Based on (pretty much char for char) Herb Sutter's GoTW #29.
// (see http://www.gotw.ca/gotw/029.htm)
//
#ifndef BOOST_CGI_COMMON_NAME_HPP_INCLUDED__
#define BOOST_CGI_COMMON_NAME_HPP_INCLUDED__

#include <string>
//#include <ostream>
#include <cstdlib>
#include <ctype.h>
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   template <typename CharT>
   struct ichar_traits
     : std::char_traits<CharT>
   {
     static bool eq(char c1, char c2)
     { return toupper(c1) == toupper(c2); }

     static bool ne(char c1, char c2)
     { return toupper(c1) != toupper(c2); }

     static bool lt(char c1, char c2)
     { return toupper(c1) < toupper(c2); }

     static int compare( const char* str1
                       , const char* str2
                       , std::size_t num )
     {
       if (num)
         do {
           int u1 = std::toupper(*str1);
           int u2 = std::toupper(*str2);
           if (u1 != u2)
             return u1 - u2;
           ++str1;
           ++str2;
         } while (--num);

       return 0;
     }

     static const char*
       find(const char* str, int n, char a)
     {
       int u = std::toupper(a);
       while (n-- > 0) {
         if (std::toupper(*str) == u)
           return str;
         else
           ++str;
       }
       return 0;
     }

   };

   // typedefs for typical usage.
   typedef std::basic_string<char, ichar_traits<char> >       name;
   typedef std::basic_string<wchar_t, ichar_traits<wchar_t> > wname;

  // Allow output using standard streams (conserves original case).
  template <typename CharT, typename Traits>
  std::basic_ostream<CharT, Traits>&
    operator<< (std::basic_ostream<CharT, Traits>& os
               , std::basic_string<CharT, ichar_traits<CharT> > const& str)
  {
    return os<< str.c_str();
  }

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_COMMON_NAME_HPP_INCLUDED__

