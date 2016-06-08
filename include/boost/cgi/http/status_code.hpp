//               -- http/status_code.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_HTTP_STATUS_CODE_HPP_INCLUDED__
#define CGI_HTTP_STATUS_CODE_HPP_INCLUDED__

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {
  namespace http {

    /// Standard HTTP status codes
    /**
     * See http://tools.ietf.org/html/rfc2616#section-10
     */
    enum status_code
    {
      /// Informational codes
      continue_                      = 100, // note the trailing underscore
      switching_protocols,
      processing,

      /// Success codes
      ok                             = 200,
      created,
      accepted,
      non_authorative_information,
      no_content,
      reset_content,
      partial_content,
      multi_status,

      /// Redirect codes
      multiple_choices               = 300,
      moved_permanently,
      found,
      see_other,
      not_modified,
      use_proxy,
      switch_proxy,
      temporary_redirect,

      /// Domain error codes
      bad_request                    = 400,
      unauthorized,
      payment_required,
      forbidden,
      not_found,
      method_not_allowed,
      not_acceptable,
      proxy_authentication_required,
      request_timeout,
      conflict,
      gone,
      length_required,
      precondition_failed,
      request_entity_too_large,
      request_uri_too_long,
      unsupported_media_type,
      request_range_not_satisfiable,
      expectation_failed,
      unprocessable_entity           = 422,
      locked,
      failed_dependency,
      unordered_collection,
      upgrade_required,
      retry_with                     = 449,

      /// Internal error codes
      internal_server_error          = 500,
      not_implemented,
      bad_gateway,
      service_unavailable,
      gateway_timeout,
      http_version_not_supported,
      insufficient_storage,
      bandwidth_limit_exceeded       = 509
    };

  } // namespace http
 } // namespace common

BOOST_CGI_NAMESPACE_END

namespace boost { namespace http {
  using namespace ::BOOST_CGI_NAMESPACE::common::http;
} } // namespace boost::http

#endif // CGI_HTTP_STATUS_CODE_HPP_INCLUDED__

