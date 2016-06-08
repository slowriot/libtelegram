
#ifndef BOOST_CGI_FWD_SESSIONS_HPP_INCLUDED_
#define BOOST_CGI_FWD_SESSIONS_HPP_INCLUDED_

#include "boost/cgi/config.hpp"
#include <boost/uuid/uuid_generators.hpp>

BOOST_CGI_NAMESPACE_BEGIN
  namespace common {
    template<typename T> class basic_session;
    template<typename Protocol> class basic_session_manager;
  }
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_FWD_SESSIONS_HPP_INCLUDED_