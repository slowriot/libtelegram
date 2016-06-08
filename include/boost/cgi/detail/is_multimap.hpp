//                   is_multimap.hpp --
//
//           Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_IS_MULTIMAP_INCLUDED_20100430_
#define BOOST_CGI_IS_MULTIMAP_INCLUDED_20100430_

BOOST_CGI_NAMESPACE_BEGIN
  namespace detail {

    template<typename T>
    struct is_multimap
      : boost::mpl::false_
    {
    };

    template<typename T, typename U>
    struct is_multimap< std::multimap<T,U> >
      : boost::mpl::true_
    {
    };

  } // namespace detail
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_IS_MULTIMAP_INCLUDED_20100430_

