//            -- detail/basic_io_object.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_BASIC_IO_OBJECT_HPP_INCLUDED__
#define CGI_DETAIL_BASIC_IO_OBJECT_HPP_INCLUDED__

#include <boost/asio/basic_io_object.hpp>
#include <boost/noncopyable.hpp>
#include <boost/type_traits/is_same.hpp>
#include "boost/cgi/common/is_async.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/common/has_hidden_io_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN

 namespace detail {

  template<typename Service, typename Enable = void>
  class basic_io_object
    : private boost::noncopyable
  {
  public:
    typedef Service                                    service_type;
    typedef typename service_type::implementation_type implementation_type;

    /// Get the io_service associated with the object.
    /**
     * This function may be used to obtain the io_service object that the I/O
     * object uses to dispatch handlers for asynchronous operations.
     *
     * @return A reference to the io_service object that the I/O object will 
     * use to dispatch handlers. Ownership is not transferred to the caller.
     */
    boost::asio::io_service& get_io_service()
    {
      return service.get_io_service();
    }

  protected:
    explicit basic_io_object()
    {
      service.construct(implementation);
    }

    ~basic_io_object()
    {
      service.destroy(implementation);
    }

    service_type service;
    implementation_type implementation;
  };
  
  /// basic_io_object alternative when an io_service isn't used
  template<typename Service>
  class basic_io_object<
      Service,
      typename boost::enable_if<
        boost::mpl::and_<
          common::is_async<typename Service::protocol_type>,
          boost::mpl::not_<
            boost::is_same<
              typename Service::protocol_type, common::tags::cgi
            >
          >
        >
      >::type
    >
    : private boost::noncopyable
  {
  public:
    typedef Service                                  service_type;
    typedef typename Service::implementation_type    implementation_type;

    /// Get the io_service associated with the object.
    /**
     * This function may be used to obtain the io_service object that the I/O
     * object uses to dispatch handlers for asynchronous operations.
     *
     * @return A reference to the io_service object that the I/O object will 
     * use to dispatch handlers. Ownership is not transferred to the caller.
     */
    boost::asio::io_service& get_io_service()
    {
      return service.get_io_service();
    }

  protected:
    /// Construct a basic_io_object.
    /**
     * Performs:
     * @code service.construct(implementation); @endcode
     */
    explicit basic_io_object(boost::asio::io_service& io_service)
      : service(boost::asio::use_service<service_type>(io_service))
    {
      service.construct(implementation);
    }

    ~basic_io_object()
    {
      service.destroy(implementation);
    }

    service_type& service;
    implementation_type implementation;
  };
  
  /// basic_io_object alternative when an io_service isn't used
  template<typename Service>
  class basic_io_object<
      Service,
      typename boost::enable_if<
        boost::is_same<
          typename Service::protocol_type, common::tags::cgi
        >
      >::type
    >
    : private boost::noncopyable
  {
  public:
    typedef Service                                  service_type;
    typedef typename Service::implementation_type    implementation_type;

    /// Get the io_service associated with the object.
    /**
     * This function may be used to obtain the io_service object that the I/O
     * object uses to dispatch handlers for asynchronous operations.
     *
     * @return A reference to the io_service object that the I/O object will 
     * use to dispatch handlers. Ownership is not transferred to the caller.
     */
    boost::asio::io_service& get_io_service()
    {
      return service.get_io_service();
    }

  protected:
    /// Construct a basic_io_object.
    /**
     * Performs:
     * @code service.construct(implementation); @endcode
     */
    explicit basic_io_object()
      : ios()
      , service(boost::asio::use_service<service_type>(ios))
    {
      service.construct(implementation);
    }

    ~basic_io_object()
    {
      service.destroy(implementation);
    }

    boost::asio::io_service ios;
    service_type& service;
    implementation_type implementation;
  };
  
  
  
 } // namespace detail
BOOST_CGI_NAMESPACE_END

#endif // CGI_DETAIL_BASIC_IO_OBJECT_HPP_INCLUDED__
