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

  /// Hold a set number of io_contexts and return them based on the Policy
  /**
   * Contains six member functions:
   * io_context() - returns a reference to an io_context
   * stop()       - stops all io_contexts
   * run()        - runs all io_contexts
   * reset()      - resets all io_contexts

   * (strike) stop_one()   - stops one (running) io_context
   * (strike) run_one()    - runs one (not-yet-started/reset) io_context
   *
   * It can also be set up so that only required io_contexts are run:
   * each call to io_context() can get an io_context, call io_context::run() on
   * it and then return it.
   */
  template<typename PoolingPolicy>
  class io_context_provider
    : private boost::noncopyable
  {
  public:
    io_context_provider(int)
      : io_context_()
    {
    }

    io_context_provider()
      : io_context_()
    {
    }

    ::BOOST_CGI_NAMESPACE::common::io_context&
      get_io_context()
    {
      return io_context_;
    }

    void run()
    {
      io_context_.run();
    }

    void stop()
    {
      io_context_.stop();
    }

    void reset()
    {
      io_context_.restart();
    }
  private:
    ::BOOST_CGI_NAMESPACE::common::io_context io_context_;
  };


  /*
  template<>
  class io_context_provider<tags::service_pool>
  {
  public:
    typedef std::list<BOOST_CGI_NAMESPACE::io_context> impl_type;

    io_context_provider(int pool_size)
      : io_contexts_(pool_size)
      , current_(io_contexts_.begin())
    {
    }

    BOOST_CGI_NAMESPACE::io_context& io_context()
    {
      return boost::ref(*current_++);
    }

    void run()
    {
      std::for_each(io_contexts_.begin(), io_contexts_.end()
                    , boost::bind(&BOOST_CGI_NAMESPACE::io_context::run, boost::ref(_1)));
    }

    void stop()
    {
      std::for_each(io_contexts_.begin(), io_contexts_.end()
                    , boost::bind(&BOOST_CGI_NAMESPACE::io_context::stop, boost::ref(_1)));
    }

    void reset()
    {
      std::for_each(io_contexts_.begin(), io_contexts_.end()
                    , boost::bind(&BOOST_CGI_NAMESPACE::io_context::reset, boost::ref(_1)));
    }

    impl_type& impl()
    {
      return io_contexts_;
    }

  private:
    impl_type io_contexts_;
    impl_type::iterator current_;
  };
  */

  /// Specialization for multi-queue/single-io_context strategy
  /**
   * Holds a reference to a passed in io_context. Use of this requires the
   * user passes in an io_context on construction of the
   * basic_protocol_service<>.
   */
/********************************
  template<typename Policy>
  class io_context_provider<0, Policy>
    : private boost::noncopyable
  {
  public:
    io_context_provider(boost::asio::io_context& ios)
      : io_context_(ios)
    {
    }

    boost::asio:io_context& io_context()
    {
      return io_context_;
    }

    void run()
    {
      io_context_.run();
    }

    void stop()
    {
      io_context_.stop();
    }

    void reset()
    {
      io_context_.reset();
    }
  private:
    boost::asio::io_context& io_context_;
  };
********************************/

      /*
  /// Specialization for io_context-per-queue strategy
  template<typename Policy>
  class io_context_provider<1, Policy>//tags::>
    : private boost::noncopyable
  {
  public:
    io_context_provider(int)
      : io_context_()
      , work_(io_context_)
    {
    }

    io_context_provider()
      : io_context_()
      , work_(io_context_)
    {
    }

    boost::asio::io_context& io_context()
    {
      return io_context_;
    }
  private:
    boost::asio::io_context io_context_;
    boost::asio::io_context::work work_;
  };



  /// Specialization for a variable number of io_contexts
  template<typename Policy>
  class io_context_provider<-1, Policy>//tags::round_robin_pool>
    : private boost::noncopyable
  {
    typedef boost::shared_ptr<boost::asio::io_context> io_context_ptr;
    typedef boost::shared_ptr<boost::asio::io_context::work> work_ptr;

  public:
    io_context_provider(int pool_size = 7)
      : pos_(0)
      , io_contexts_()
//    , strand_(io_contexts_[0])
    {
      if (pool_size == 0)
        throw std::runtime_error("io_context_pool size is 0");

      // Give all the io_contexts work to do so that their run() functions will
      // not exit until they are explicitly stopped.
      for (std::size_t i = 0; i < pool_size; ++i)
      {
        io_context_ptr io_context(new boost::asio::io_context);
        work_ptr work(new boost::asio::io_context::work(*io_context));
        io_contexts_.push_back(io_context);
        work_.push_back(work);
      }
    }

    void run()
    {
      // Create a pool of threads to run all of the io_contexts.
      std::vector<boost::shared_ptr<boost::thread> > threads;
      for (std::size_t i = 0; i < io_contexts_.size(); ++i)
      {
        boost::shared_ptr<boost::thread>
          thread(new boost::thread(boost::bind(&boost::asio::io_context::run
                                              , io_contexts_[i])));
        threads.push_back(thread);
      }

      // Wait for all threads in the pool to exit.
      for (std::size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();
    }

    void stop()
    {
      // Explicitly stop all io_contexts.
      for (std::size_t i = 0; i < io_contexts_.size(); ++i)
        io_contexts_[i]->stop();
    }

    // NOT THREAD-SAFE (but should be)
    boost::asio::io_context& io_context()
    {
      boost::asio::io_context& io_context = *io_contexts_[pos_];
      if (++pos_ == io_contexts_.size())
        pos_ = 0;
      return io_context;
    }
  private:
    int pos_;
    std::vector<io_context_ptr> io_contexts_;
    std::vector<work_ptr> work_;

    //boost::asio::io_context::strand strand_;
  };
      */
 } // namespace common

 // **FIXME**
 using common::io_context_provider;

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_IO_SERVICE_PROVIDER_HPP_INCLUDED__

