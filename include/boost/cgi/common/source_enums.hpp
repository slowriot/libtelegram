
#ifndef BOOST_CGI_COMMON_SOURCE_ENUMS_HPP_INCLUDED__
#define BOOST_CGI_COMMON_SOURCE_ENUMS_HPP_INCLUDED__

#include <boost/fusion/include/at.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/config.hpp"
#include "boost/cgi/common/map.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   /// Types of data sources and related enums.
   /**
    * Each data source is given a unique type. This is so the accessor
    * operators on a `basic_request<>` are compile-time aliases to the
    * internal data maps.
    *
    * If you want to add a new data type to a request you need to:
    *   > Update this file
    *   > Update map.hpp with a new map type
    *   > Update `data_map_type` in the `request_base` class
    */

   template<typename Impl>
     env_map&
       env_vars(Impl& impl)     { return boost::fusion::at_c<0>(impl); }
   template<typename Impl>
     get_map&
       get_vars(Impl& impl)     { return boost::fusion::at_c<1>(impl); }
   template<typename Impl>
     post_map&
       post_vars(Impl& impl)    { return boost::fusion::at_c<2>(impl); }
   template<typename Impl>
     cookie_map&
       cookie_vars(Impl& impl)  { return boost::fusion::at_c<3>(impl); }
   template<typename Impl>
     upload_map&
       upload_vars(Impl& impl)  { return boost::fusion::at_c<4>(impl); }
   template<typename Impl>
     session_map&
       session_vars(Impl& impl) { return boost::fusion::at_c<5>(impl); }

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_COMMON_SOURCE_ENUMS_HPP_INCLUDED__

