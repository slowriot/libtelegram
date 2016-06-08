//                  -- service_id.hpp --
//                 (taken from Boost.Asio)
//
//     Copyright (c) 2003-2007 Christopher M. Kohlhoff
//               (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_SERVICE_ID_HPP_INCLUDED__
#define CGI_DETAIL_SERVICE_ID_HPP_INCLUDED__

#include <boost/asio/detail/push_options.hpp>

#include <boost/asio/io_service.hpp>

BOOST_CGI_NAMESPACE_BEGIN
 namespace detail {

   // Special derived service id type to keep classes header-file only.
   template <typename Type>
   class service_id
     : public boost::asio::io_service::id
   {
  };

 } // namespace detail
BOOST_CGI_NAMESPACE_END

#include <boost/asio/detail/pop_options.hpp>

#endif // CGI_DETAIL_SERVICE_ID_HPP_INCLUDED__
