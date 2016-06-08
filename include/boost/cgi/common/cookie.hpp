//                   -- cookie.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_COOKIE_HPP_INCLUDED__
#define CGI_COOKIE_HPP_INCLUDED__

#include <string>
#include <boost/system/error_code.hpp>
#include <boost/tokenizer.hpp>
#include "boost/cgi/common/name.hpp"
#include "boost/cgi/config.hpp"

#ifndef BOOST_CGI_DATE_IN_THE_PAST
#  define BOOST_CGI_DATE_IN_THE_PAST "Fri, 05-Jun-1989 15:30:00 GMT"
#endif // BOOST_CGI_DATE_IN_THE_PAST

BOOST_CGI_NAMESPACE_BEGIN

 namespace common {

  template<typename CharT> struct basic_cookie;

  // typedefs for common usage
  typedef basic_cookie<char>    cookie;
  typedef basic_cookie<wchar_t> wcookie;

  /// A `basic_cookie<>` object that can be (out-) streamed
  /**
   * Either set the parameters in the constructor, or set them directly.
   * Note: If you want to set the parameters individually, remember that
   * each parameter must NOT have a trailing semi-colon!
   *
   * TODO
   * - Data should be URL-encoded, or maybe provide an overload for url_decode
   *   that takes an HttpCookie?
   * - Add from_string() ?
   */
  template<typename CharT>
  struct basic_cookie
  {
    typedef basic_cookie<CharT>               self_type;
    typedef CharT                             char_type;
    typedef typename std::basic_string<CharT> string_type;

    basic_cookie() {}
    basic_cookie(const char* _name)
      : name(_name)
      , value()
      , expires(BOOST_CGI_DATE_IN_THE_PAST)
      , path("/")
      , secure(false)
      , http_only(false)
    {
    }

    /// Create a cookie for deleting the cookie named `_name`.
    basic_cookie(const string_type& _name)
      : name(_name)
      , value()
      , expires(BOOST_CGI_DATE_IN_THE_PAST)
      , path("/")
      , secure(false)
      , http_only(false)
    {
    }

    /// Create a cookie.
    basic_cookie(const string_type& _name, const string_type& _val
                , const string_type& _path = "/"
                , const string_type& _expires = ""
                , const string_type& _domain = ""
                , bool _secure = false
                , bool HttpOnly = false)
      : name(_name)
      , value(_val)
      , expires(_expires)
      , path(_path)
      , domain(_domain)
      , secure(_secure)
      , http_only(HttpOnly)
    {
    }

    string_type name;
    string_type value;
    string_type expires;
    string_type path;
    string_type domain;
    /// `true` iff the cookie is only used for secure connections.
    bool secure;
    bool http_only;

    /// TODO: Create a cookie from a const char*
    /**
     * Rules at: http://wp.netscape.com/newsref/std/cookie_spec.html
     */

    /// Make a string out of the cookie.
    string_type to_string() const
    {
      string_type str(name + "=" + value);
      if (!expires.empty()) str += ("; expires=" + expires);
      if (!path.empty()   ) str += ("; path="    + path);
      if (!domain.empty() ) str += ("; domain="  + domain);
      if ( secure         ) str += "; secure";
      if ( http_only      ) str += "; HttpOnly";
      return str;
    }
    
    bool empty() const { return value.empty(); }
    
    const char_type* c_str() const { return value.c_str(); }
    operator string_type const& () const { return value; }
    operator string_type () const { return value; }
    
    bool operator==(string_type const& other) { return value == other; }
    
    template<typename T>
    friend std::ostream& operator<<(std::ostream& os, basic_cookie<T> const& ck);
  };

  inline
  std::pair<common::name, common::cookie>
    make_pair (const char* n, common::cookie& ck)
  {
    common::cookie new_cookie(n, ck.name);
    return std::make_pair(n, new_cookie);
  }

  template<typename CharT>
  inline std::ostream& operator<< (std::ostream& os, BOOST_CGI_NAMESPACE::common::basic_cookie<CharT> const& ck)
  {
    os<< ck.value;
    return os;
  }
  
 } // namespace common

BOOST_CGI_NAMESPACE_END


#endif // CGI_COOKIE_HPP_INCLUDED__

