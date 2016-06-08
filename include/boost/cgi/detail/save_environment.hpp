//           -- detail/save_environment.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_SAVE_ENVIRONMENT_HPP_INCLUDED__
#define CGI_DETAIL_SAVE_ENVIRONMENT_HPP_INCLUDED__

#include <map>
#include <string>
#include <cstdlib>
#include "boost/cgi/common/map.hpp"

// The process' environment
#if BOOST_WINDOWS
  // MSVC warns of 'inconsistent dll linkage' here...
  _CRTIMP extern char** _environ;
#else
  extern char** environ;
#endif


BOOST_CGI_NAMESPACE_BEGIN
 namespace detail {

   /// Save all information from `environment` to env_map
   /**
    * @param env This defaults to `::environ`, or the current process'
    *            environment.
	*
	* Note: empty variables are not guaranteed to be set by the server, so
    * we are free to ignore them too. Whether we do or not depends on the
	* macro: BOOST_CGI_KEEP_EMPTY_VARS
    */
   template<typename MapT>
   void save_environment(MapT& env_map, char** env =
       // Windows calls the environment _environ
#      if BOOST_WINDOWS
         _environ
#      else
	     environ
#      endif
     )
   {
    
     BOOST_ASSERT(env && "Trying to save environment, but the passed in environment is empty / invalid.");

     typename MapT::key_type sa;
     typename MapT::mapped_type sb;

     for(; env && *env; ++env)
     {
       int i=0;
       int j=int(strlen(*env));
       for(; i < j; ++i)
         if ((*env)[i] == '=')
           break;

#if defined(BOOST_CGI_KEEP_EMPTY_VARS)
       sa.assign(*env, i);
       if ((*env)[i+1] != '\0')
         sb.assign((*env+i+1), j-i-1);
       else
         sb.clear();
       env_map[sa] = sb;
#else
       if ((*env)[i+1] != '\0')
       {
         sa.assign(*env, i);
         sb.assign((*env+i+1), j-i-1);
         env_map[sa] = sb;
       }
#endif
	 }
   }

 } // namespace detail
BOOST_CGI_NAMESPACE_END

#endif // CGI_DETAIL_SAVE_ENVIRONMENT_HPP_INCLUDED__
