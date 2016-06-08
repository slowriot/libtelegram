//                -- common_headers.hpp --
//
//           Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_COMMON_HEADERS_HPP_INCLUDED__
#define CGI_DETAIL_COMMON_HEADERS_HPP_INCLUDED__

// #include all protocol-independent headers only. Protocol-specific
// headers can just include this after other headers.

//#include "boost/cgi/logger.hpp"
#include "boost/cgi/config.hpp"
#include "boost/cgi/basic_request.hpp"
#include "boost/cgi/basic_request_acceptor.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/commit.hpp"
#include "boost/cgi/common/response.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/common/parse_options.hpp"
#include "boost/cgi/http/status_code.hpp"
#include "boost/cgi/import/buffer.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/import/read.hpp"
#include "boost/cgi/import/streambuf.hpp"
#include "boost/cgi/import/write.hpp"
#include "boost/cgi/utility/redirect.hpp"
#ifdef BOOST_CGI_ENABLE_SESSIONS
#  include "boost/cgi/utility/sessions.hpp"
#endif // BOOST_CGI_ENABLE_SESSIONS

#endif // CGI_DETAIL_COMMON_HEADERS_HPP_INCLUDED__

