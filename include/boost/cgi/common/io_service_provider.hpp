//              -- io_service_provider.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_IO_SERVICE_PROVIDER_HPP_INCLUDED__
#define CGI_IO_SERVICE_PROVIDER_HPP_INCLUDED__

#if _MSC_VER > 1020
#pragma once
#endif

#include "boost/cgi/detail/push_options.hpp"

#include <boost/noncopyable.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/fwd/io_service_provider_fwd.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// Hold a set number of io_services and return them based on the Policy
  /**
   * Contains six member functions:
   * io_service() - returns a reference to an io_service
   * stop()       - stops all io_services
   * run()        - runs all io_services
   * reset()      - resets all io_services

   * (strike) stop_one()   - stops one (running) io_service
   * (strike) run_one()    - runs one (not-yet-started/reset) io_service
   *
   * It can also be set up so that only required io_services are run:
   * each call to io_service() can get an io_service, call io_service::run() on
   * it and then return it.
   */
  template<typename PoolingPolicy>
  class io_service_provider
    : private boost::noncopyable
  {
  public:
    io_service_provider(int)
      : io_service_()
    {
    }

    io_service_provider()
      : io_service_()
    {
    }

    ::BOOST_CGI_NAMESPACE::common::io_service&
      get_io_service()
    {
      return io_service_;
    }

    void run()
    {
      io_service_.run();
    }

    void stop()
    {
      io_service_.stop();
    }

    void reset()
    {
      io_service_.reset();
    }
  private:
    ::BOOST_CGI_NAMESPACE::common::io_service io_service_;
  };


  /*
  template<>
  class io_service_provider<tags::service_pool>
  {
  public:
    typedef std::list<BOOST_CGI_NAMESPACE::io_service> impl_type;

    io_service_provider(int pool_size)
      : io_services_(pool_size)
      , current_(io_services_.begin())
    {
    }

    BOOST_CGI_NAMESPACE::io_service& io_service()
    {
      return boost::ref(*current_++);
    }

    void run()
    {
      std::for_each(io_services_.begin(), io_services_.end()
                    , boost::bind(&BOOST_CGI_NAMESPACE::io_service::run, boost::ref(_1)));
    }

    void stop()
    {
      std::for_each(io_services_.begin(), io_services_.end()
                    , boost::bind(&BOOST_CGI_NAMESPACE::io_service::stop, boost::ref(_1)));
    }

    void reset()
    {
      std::for_each(io_services_.begin(), io_services_.end()
                    , boost::bind(&BOOST_CGI_NAMESPACE::io_service::reset, boost::ref(_1)));
    }

    impl_type& impl()
    {
      return io_services_;
    }

  private:
    impl_type io_services_;
    impl_type::iterator current_;
  };
  */

  /// Specialization for multi-queue/single-io_service strategy
  /**
   * Holds a reference to a passed in io_service. Use of this requires the
   * user passes in an io_service on construction of the
   * basic_protocol_service<>.
   */
/********************************
  template<typename Policy>
  class io_service_provider<0, Policy>
    : private boost::noncopyable
  {
  public:
    io_service_provider(boost::asio::io_service& ios)
      : io_service_(ios)
    {
    }

    boost::asio:io_service& io_service()
    {
      return io_service_;
    }

    void run()
    {
      io_service_.run();
    }

    void stop()
    {
      io_service_.stop();
    }

    void reset()
    {
      io_service_.reset();
    }
  private:
    boost::asio::io_service& io_service_;
  };
********************************/

      /*
  /// Specialization for io_service-per-queue strategy
  template<typename Policy>
  class io_service_provider<1, Policy>//tags::>
    : private boost::noncopyable
  {
  public:
    io_service_provider(int)
      : io_service_()
      , work_(io_service_)
    {
    }

    io_service_provider()
      : io_service_()
      , work_(io_service_)
    {
    }

    boost::asio::io_service& io_service()
    {
      return io_service_;
    }
  private:
    boost::asio::io_service io_service_;
    boost::asio::io_service::work work_;
  };



  /// Specialization for a variable number of io_services
  template<typename Policy>
  class io_service_provider<-1, Policy>//tags::round_robin_pool>
    : private boost::noncopyable
  {
    typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
    typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

  public:
    io_service_provider(int pool_size = 7)
      : pos_(0)
      , io_services_()
//    , strand_(io_services_[0])
    {
      if (pool_size == 0)
        throw std::runtime_error("io_service_pool size is 0");

      // Give all the io_services work to do so that their run() functions will
      // not exit until they are explicitly stopped.
      for (std::size_t i = 0; i < pool_size; ++i)
      {
        io_service_ptr io_service(new boost::asio::io_service);
        work_ptr work(new boost::asio::io_service::work(*io_service));
        io_services_.push_back(io_service);
        work_.push_back(work);
      }
    }

    void run()
    {
      // Create a pool of threads to run all of the io_services.
      std::vector<boost::shared_ptr<boost::thread> > threads;
      for (std::size_t i = 0; i < io_services_.size(); ++i)
      {
        boost::shared_ptr<boost::thread>
          thread(new boost::thread(boost::bind(&boost::asio::io_service::run
                                              , io_services_[i])));
        threads.push_back(thread);
      }

      // Wait for all threads in the pool to exit.
      for (std::size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();
    }

    void stop()
    {
      // Explicitly stop all io_services.
      for (std::size_t i = 0; i < io_services_.size(); ++i)
        io_services_[i]->stop();
    }

    // NOT THREAD-SAFE (but should be)
    boost::asio::io_service& io_service()
    {
      boost::asio::io_service& io_service = *io_services_[pos_];
      if (++pos_ == io_services_.size())
        pos_ = 0;
      return io_service;
    }
  private:
    int pos_;
    std::vector<io_service_ptr> io_services_;
    std::vector<work_ptr> work_;

    //boost::asio::io_service::strand strand_;
  };
      */
 } // namespace common

 // **FIXME**
 using common::io_service_provider;
  
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_IO_SERVICE_PROVIDER_HPP_INCLUDED__

