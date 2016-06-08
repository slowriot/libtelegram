#ifndef BOOST_CGI_COMMON_MAP_HPP_INCLUDED__
#define BOOST_CGI_COMMON_MAP_HPP_INCLUDED__

#include <map>
#include <boost/mpl/bool.hpp>
#include "boost/cgi/common/name.hpp"
#include "boost/cgi/common/cookie.hpp"
#include "boost/cgi/common/form_part.hpp"

/// You can define your 
#ifndef BOOST_CGI_SESSION_MAP_TYPE
#  define BOOST_CGI_SESSION_MAP_TYPE map
#endif // BOOST_CGI_SESSION_MAP_TYPE

#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  typedef std::map< ::BOOST_CGI_NAMESPACE::common::name, std::string> map;
  typedef std::multimap< ::BOOST_CGI_NAMESPACE::common::name, std::string> multimap;

   /**
    * If you want to add a new data type to a request you need to:
    *   > Update this file
    *   > Update source_enums.hpp
    *   > Update `data_map_type` in the `request_base` class
    */
  typedef map        env_map;
  typedef multimap   get_map;
  typedef multimap   post_map;
  typedef multimap   form_map;
  typedef map        cookie_map;
  typedef std::multimap<
      ::BOOST_CGI_NAMESPACE::common::name,
      ::BOOST_CGI_NAMESPACE::common::form_part
  >                  upload_map;
  typedef map        session_map;

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_COMMON_MAP_HPP_INCLUDED__
