//            -- basic_protocol_service.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_PROTOCOL_SERVICE_HPP_INCLUDED__
#define CGI_BASIC_PROTOCOL_SERVICE_HPP_INCLUDED__

#include <set>
#include <queue>
///////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include <boost/asio/strand.hpp>
#include <boost/detail/workaround.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/config.hpp"
#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/common/io_service_provider.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/fwd/basic_protocol_service_fwd.hpp"
#include "boost/cgi/import/io_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// Basic Protocol Service
  /**
   * Holds the request queue and the connection queue.
   * It is also a wrapper around asio::io_context
   */
  template<typename Protocol, typename IoContextProvider>
  class basic_protocol_service
  {
  public:
    typedef Protocol                                 protocol_type;
    typedef IoContextProvider                        ios_provider_type;
    typedef typename protocol_traits<Protocol>::type traits;
    typedef typename traits::string_type             string_type;
    typedef typename traits::request_type            request_type;
    typedef boost::shared_ptr<request_type>          request_ptr;
    typedef std::set<request_ptr>                    set_type;
    typedef std::queue<request_ptr>                  queue_type;

    basic_protocol_service(int pool_size_hint = 1)
      : ios_provider_(pool_size_hint)
      , request_set_()
      , request_queue_()
    {
    }

    basic_protocol_service(boost::asio::io_context& ios)
      : ios_provider_(ios)
      , request_set_()
      , request_queue_()
    {
    }

    ~basic_protocol_service()
    {
    }

    /// Run all the io_contexts contained by this service
    /**
     * This is equivalent to calling run() on each of the io_contexts held by
     * ios_provider_
     */
    void run()
    {
      ios_provider_.run();
    }

    /// Stop all the io_contexts contained by this service
    /**
     * This is equivalent to calling stop() on each of the io_contexts held by
     * ios_provider_
     */
    void stop()
    {
      ios_provider_.stop();
    }

    /// Reset all the io_contexts contained by this service
    /**
     * This deletes the request queue(s), aborts all running requests and then
     * calls reset() on each of the io_contexts held by ios_provider_. There is
     * no guarantee that requests will terminate immediately.
     */
    void reset()
    {
      request_queue_.clear();
      request_set_.clear();
      ios_provider_.reset();
    }

    bool is_cgi()
    {
      return true;
    }

    /// Return an available io_context from the IoContextProvider
    /**
     * The order in which the underlying io_contexts are returned is determined
     * by what policy the IoContextProvider uses.
     */
    ::BOOST_CGI_NAMESPACE::common::io_context& get_io_context()
    {
      return ios_provider_.get_io_context();
    }

    /// Post the handler through an available io_context
    template<typename Handler>
    void post(Handler handler)
    {
      ios_provider_.get_io_context().post(handler);
    }

    /// Dispatch a handler through an available io_context
    template<typename Handler>
    void dispatch(Handler handler)
    {
      ios_provider_.get_io_context().dispatch(handler);
    }

  private:
    ios_provider_type ios_provider_;

    /// A std::set of all the requests.
    set_type request_set_;
    /// A std::queue of the waiting (ie. not-being-handled) requests.
    queue_type request_queue_;

#if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1400))
    friend typename traits::request_type;
#else
    // GCC && GCC < 4.7
    #if defined(__GNUC__) && !defined(__clang__) && defined(__GNUC_MINOR__) && ((__GNUC__ << 16) + __GNUC_MINOR__ < ((4) << 16) + (7))
        friend class traits::request_type;
    #else
        friend typename traits::request_type;
    #endif
#endif
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_BASIC_PROTOCOL_SERVICE_HPP_INCLUDED__
