
#ifndef BOOST_CGI_REDIRECT_HPP_INCLUDED_
#define BOOST_CGI_REDIRECT_HPP_INCLUDED_

#include "boost/cgi/detail/push_options.hpp"

#include <string>
///////////////////////////////////////////////////////////
#include <boost/system/error_code.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/import/write.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/detail/throw_error.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   /// Redirect a request to another place.
   /**
    * Note that without also outputting a content-type header, this will be a
    * 'soft' (internal) redirect. If you want to do a 'hard' redirect, pass a
    * 'Content-type: text/plain' header along with two CRLFs to complete the
    * headers.
    */
   template <typename RequestT>
   basic_header<typename RequestT::char_type>
   redirect(RequestT& req, typename RequestT::string_type const& dest
           , bool secure, boost::system::error_code& ec)
   {
     typedef typename RequestT::string_type string_type;
     string_type url(dest);
     if (url.find("://") == string_type::npos) {
       url = secure ? "https" : "http";
       url += "://" + req.server_name();
       if (dest[0] == '/')
         url += dest;
       else
         url += "/" + dest;
     }
     return basic_header<typename RequestT::char_type>("Location", url);
   }

   template <typename RequestT>
   basic_header<typename RequestT::char_type>
   redirect(RequestT& req, typename RequestT::string_type const& dest, bool secure = false)
   {
     boost::system::error_code ec;
     basic_header<typename RequestT::char_type>
         hdr(redirect(req, dest, secure, ec));
     detail::throw_error(ec);
     return hdr;
   }

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_REDIRECT_HPP_INCLUDED_

