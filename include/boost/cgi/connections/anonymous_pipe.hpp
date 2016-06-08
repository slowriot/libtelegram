//               -- anonymous_pipe.hpp --
//
//          Copyright (c) Darren Garvey 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_ANONYMOUS_PIPE_INCLUDED_20100204
#define BOOST_CGI_ANONYMOUS_PIPE_INCLUDED_20100204

#include <algorithm>
#include <deque>
#include <memory>

#include <boost/atomic.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/lock_types.hpp> 
#include <boost/thread/mutex.hpp>
#include <boost/version.hpp>
#include "boost/cgi/error.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/basic_connection.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/push_options.hpp"
#include "boost/cgi/common/connection_base.hpp"
#include "boost/cgi/common/protocol_traits.hpp"

//////////////////////////////////////////////////////////////////////////

BOOST_CGI_NAMESPACE_BEGIN
  namespace common {

  /// Specialisation of basic_connection for anonymous pipes on Windows.
  /**
   * For default initialisation of FastCGI to work on Windows with mod_fcgid
   * and Apache, we need to support anonymous pipes. This class is used to
   * allow the support.
   */
  template<>
  class basic_connection<tags::anonymous_pipe>
    : public connection_base
  {
    enum { max_buffers = 64 };
  public:
    typedef basic_connection<tags::anonymous_pipe> self_type;
    typedef boost::shared_ptr<self_type>           pointer;
    typedef HANDLE native_handle_type;

      basic_connection(boost::asio::io_service& ios)
      : io_service(ios)
      , file_handle(INVALID_HANDLE_VALUE)
      , do_io_(true)
    {
        io_thread_.reset( new boost::thread(boost::bind(&basic_connection::io_worker, this)));
    }

    virtual ~basic_connection()
    {
        do_io_ = false;
        {
            lock_t lock(io_mutex_);
            read_queue_.clear();
            write_queue_.clear();
        }
        io_.notify_one();
        
        close();
        io_thread_->join();
    }

    static pointer create(boost::asio::io_service& ios)
    {
      return pointer(new self_type(ios));
    }

    void assign(native_handle_type const &handle)
    {
        boost::system::error_code ec;
        assign(handle, ec);
    }

    boost::system::error_code assign(native_handle_type const &handle, boost::system::error_code &ec)
    {
      if (is_open())
        ec = close(ec);

      if (ec)
        return ec;

      file_handle = handle;
      return ec;
    }

    bool is_open() { return file_handle != INVALID_HANDLE_VALUE; }

    void close()
    {
      boost::system::error_code ec;
      close(ec);
    }

    boost::system::error_code close(boost::system::error_code &ec)
    {
      /*
       * Make sure that the client (ie. a Web Server in this case) has
       * read all data from the pipe before we disconnect.
       */
      if (!::FlushFileBuffers(file_handle))
      {
        ec = boost::system::error_code(::GetLastError(), boost::system::system_category());
        return ec;
      }

      if (!::DisconnectNamedPipe(file_handle))
      {
        ec =  boost::system::error_code(::GetLastError(), boost::system::system_category());
        return ec;
      }

      file_handle = INVALID_HANDLE_VALUE;
      return ec;
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buffers)
    {
      boost::system::error_code ec;
      std::size_t bytes_read = read_some(buffers, ec);
      detail::throw_error(ec);
      return bytes_read;
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buffers
                         , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      typename MutableBufferSequence::const_iterator iter = buffers.begin();
      typename MutableBufferSequence::const_iterator end = buffers.end();
      DWORD i = 0;
      size_t buffer_size = 0;
      size_t bytes_transferred = 0;
      size_t total_bytes_transferred = 0;
      for (; !ec && iter != end && i < max_buffers; ++iter, ++i)
      {
        boost::asio::mutable_buffer buffer(*iter);
        buffer_size = boost::asio::buffer_size(buffer);
        bytes_transferred = read_some(
          boost::asio::buffer_cast<char*>(buffer), buffer_size , ec);
        if (bytes_transferred < buffer_size)
          return bytes_transferred;
        total_bytes_transferred += buffer_size;
      }
      return total_bytes_transferred;
    }

    std::size_t read_some(char* buf, std::size_t len
        , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      DWORD bytesRead;
      int ret = -1;

      if (::ReadFile(file_handle, buf, DWORD(len), &bytesRead, NULL))
        ret = bytesRead;
      else
        ec = boost::system::error_code(
                ::GetLastError(), 
#if BOOST_VERSION < 104400
                boost::system::system_category);
#else
                boost::system::system_category());
#endif

      return ret;
    }

    template<typename MutableBufferSequence, typename ReadHandler>
    void async_read_some(const MutableBufferSequence &buffers, ReadHandler handler)
    {
      std::unique_ptr<read_context> cntxt(new read_context());
      std::copy(begin(buffers), end(buffers), std::back_inserter(cntxt->buffers));
      cntxt->handler = handler;
      {
        lock_t lock(io_mutex_);
        read_queue_.emplace_back(std::move(cntxt));
      }
      io_.notify_one();
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      std::size_t bytes_transferred(0);
      for(typename ConstBufferSequence::const_iterator i = buf.begin(),
          end (buf.end())
         ; !ec && i != end; ++i)
      {
        std::size_t buf_len = boost::asio::buffer_size(*i);
        bytes_transferred += write_some(boost::asio::buffer_cast<const char *>(*i), buf_len, ec);
      }
      return bytes_transferred;
    }

    std::size_t write_some(const char* buf, std::size_t len
                          , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      DWORD bytesWritten;
      int ret = -1;

      if (::WriteFile(file_handle, buf, DWORD(len), &bytesWritten, NULL))
        ret = bytesWritten;
      else
        ec = boost::system::error_code(
                ::GetLastError(),
#if BOOST_VERSION < 104400
                boost::system::system_category);
#else
                boost::system::system_category());
#endif

      return ret;
    }

    template<typename ConstBufferSequence, typename ReadHandler>
    void async_write_some(ConstBufferSequence &buffers, ReadHandler handler)
    {
      std::unique_ptr<write_context> cntxt(new write_context());
      std::copy(begin(buffers), end(buffers), std::back_inserter(cntxt->buffers));
      cntxt->handler = handler;
      {
        lock_t lock(io_mutex_);
        write_queue_.emplace_back(std::move(cntxt));
      }
      io_.notify_one();
    }

    boost::asio::io_service& io_service;
    native_handle_type file_handle;

    private:
      // Due to some web servers giving a pipe handle that does not have FILE_FLAG_OVERLAPPED, and not being able
      // to upgrade handle by using ReOpenFile() due to pending I/O (the client starts writing before
      // ConnectNamedPipe() even gets called) then we have to emulate it poorly with a worker thread.
      typedef boost::function<void (const boost::system::error_code&, std::size_t)> handler_t;
      typedef std::deque<boost::asio::mutable_buffer> mutable_buffers_t;
      struct read_context
      {
        mutable_buffers_t buffers;
        handler_t handler;
      };
      typedef std::deque<std::unique_ptr<read_context>> read_queue_t;

      typedef std::deque<boost::asio::const_buffer> const_buffers_t;
      struct write_context
      {
        const_buffers_t buffers;
        handler_t handler;
      };
      typedef std::deque<std::unique_ptr<write_context>> write_queue_t;

      typedef boost::unique_lock<boost::mutex> lock_t;

      read_queue_t read_queue_;
      write_queue_t write_queue_;
      boost::mutex io_mutex_;
      boost::condition_variable io_;
      boost::atomic<bool> do_io_;
      std::unique_ptr<boost::thread> io_thread_;

      void io_worker()
      {
        for(;do_io_;)
        {
          read_queue_t read_queue;
          write_queue_t write_queue;

          // Wait for work to do.
          {
            lock_t lock(io_mutex_);
            while (do_io_ && read_queue_.empty() && write_queue_.empty())
            {
              io_.wait(lock);
            }
            
            read_queue.swap(read_queue_);
            write_queue.swap(write_queue_);
          }

          if (!do_io_)
              return;

          // Write all conent before doing any reads.  Calling read blocks all calls to write, with CGI
          // request->response symantics that will result in deadlock if we wait for the next request when
          // there is still a response pending.
          do // read one.
          {
            do // all writes.
            {
              std::for_each(write_queue.begin(), write_queue.end(), [&](std::unique_ptr<write_context> const &cntxt)
              {
                  if (!do_io_)
                      return;

                  boost::system::error_code ec;
                  std::size_t bytes_transfered = write_some(cntxt->buffers, ec);
                  handler_t handler = cntxt->handler;
                  this->io_service.post([=]() -> void
                  {
                    handler(ec, bytes_transfered);
                  });
              });

              // Get any additional pending writes added in the meantime.
              write_queue.clear();
              {
                  lock_t lock(io_mutex_);
                  write_queue.swap(write_queue_);
              }
            } while (do_io_ && !write_queue.empty());

            // Read a single pending item.
            if (!read_queue.empty())
            {
              std::unique_ptr<read_context> cntxt = std::move(read_queue.front());
              read_queue.pop_front();
            
              if (!do_io_)
                return;

              boost::system::error_code ec;
              std::size_t bytes_transfered = read_some(cntxt->buffers, ec);
              handler_t handler = cntxt->handler;
              io_service.post([=]() -> void
              {
                handler(ec, bytes_transfered);
              });
            }

            // Get any additional pending reads & writes.
            {
                lock_t lock(io_mutex_);
                std::for_each(read_queue_.begin(), read_queue_.end(), [&](std::unique_ptr<read_context> &cntxt)
                {
                    read_queue.emplace_back(std::move(cntxt));
                });
                read_queue_.clear();
                write_queue.swap(write_queue_);
            }
          } while (do_io_ && (!read_queue.empty() || !write_queue.empty()));
        } // for(;;)
      } // void io_worker()
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////

#include "boost/cgi/detail/pop_options.hpp"

#endif // BOOST_CGI_ANONYMOUS_PIPE_INCLUDED_20100204
