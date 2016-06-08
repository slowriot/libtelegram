
#ifndef BOOST_CGI_COMMIT_HPP_INCLUDED_
#define BOOST_CGI_COMMIT_HPP_INCLUDED_

#include <map>
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN

 namespace common {

    /// Send a response to a reqest and close off the request.
    template<typename Request, typename Response>
    int commit(Request& req, Response& resp, int program_status = 0)
    {
      boost::system::error_code ec;
      int ret (commit(req, resp, program_status, ec));
      detail::throw_error(ec);
      return ret;
    }

    /// Send a response to a reqest and close off the request.
    /**
     * This sends the response and commits the session data.
     */
    template<typename Request, typename Response>
    int commit(Request& req, Response& resp, int program_status
              , boost::system::error_code& ec)
    {
      typedef typename Request::string_type string_type;
#ifdef BOOST_CGI_ENABLE_SESSIONS
      if (!program_status)
      {
        string_type path(
            Request::traits::session_options == path_session
              ? req.script_name()
              : "/"
          );
        if (req.session.id().empty())
          resp<< cookie(BOOST_CGI_SESSION_COOKIE_NAME, "", path, BOOST_CGI_DATE_IN_THE_PAST);
        else
          resp<< cookie(BOOST_CGI_SESSION_COOKIE_NAME, req.session.id(), path);
      }
#endif // BOOST_CGI_ENABLE_SESSIONS
      resp.send(req.client(), ec);
      return ec ? -1 : req.close(resp.status(), program_status, ec);
    }

 } // namespace common
BOOST_CGI_NAMESPACE_END

namespace boost {

  namespace cgi { using ::BOOST_CGI_NAMESPACE::common::commit; }
  namespace fcgi { using ::BOOST_CGI_NAMESPACE::common::commit; }
  namespace scgi { using ::BOOST_CGI_NAMESPACE::common::commit; }

} // namespace boost

#endif // BOOST_CGI_COMMIT_HPP_INCLUDED_

