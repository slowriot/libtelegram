//                  -- async_stdio.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_ASYNC_STDIO_HPP_INCLUDED__
#define CGI_ASYNC_STDIO_HPP_INCLUDED__

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/enable_shared_from_this.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/connections/stdio.hpp"
#include "boost/cgi/import/io_service.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  // Asynchronous access to stdio.
  /**
   * This class doesn't do real async I/O, but fakes it by posting
   * a read / write to an io_service, which may be run in a background
   * thread. On most systems this won't actually mean true async I/O,
   * but this emulates it as an interim solution.
   */
  template<>
  class basic_connection<tags::async_stdio>
    : public basic_connection<tags::stdio>
    , public boost::enable_shared_from_this<basic_connection<tags::async_stdio> >
  {
  public:
    typedef basic_connection<tags::async_stdio> type;
    typedef boost::shared_ptr<type>             pointer;

    basic_connection(common::io_service& ios)
      : basic_connection<tags::stdio>()
      , io_service_(ios)
    {
    }

    bool is_open() const
    {
      return is_open_;
    }

    void close()
    {
      is_open_ = false;
    }

    static pointer create(::BOOST_CGI_NAMESPACE::common::io_service& ios)
    {
      return pointer(new basic_connection<tags::async_stdio>(ios));
    }

    template<typename ConnectionPtr, typename MutableBufferSequence
            , typename Handler>
    class read_handler
    {
    public:
      read_handler(ConnectionPtr cptr, MutableBufferSequence& buf
                  , Handler handler)
        : cptr_(cptr)
        , buf_(buf)
        , handler_(handler)
      {
      }

      void operator()()
      {
        boost::system::error_code ec;
        std::size_t bytes_read = cptr_->read_some(buf_, ec);
        handler_(ec, bytes_read);
      }

    private:
      ConnectionPtr cptr_;
      MutableBufferSequence& buf_;
      Handler handler_;
    };

    template<typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buf, Handler handler)
    {
      io_service_.post(read_handler<pointer, MutableBufferSequence, Handler>
                         (shared_from_this(), buf, handler));
    }

    template<typename ConnectionPtr, typename ConstBufferSequence
            , typename Handler>
    class write_handler
    {
    public:
      write_handler(ConnectionPtr cptr, ConstBufferSequence& buf
                   , Handler handler)
        : cptr_(cptr)
        , buf_(buf)
        , handler_(handler)
      {
      }

      void operator()()
      {
        boost::system::error_code ec;
        std::size_t bytes_written = cptr_->write_some(buf_, ec);
        handler_(ec, bytes_written);
      }

    private:
      ConnectionPtr cptr_;
      ConstBufferSequence& buf_;
      Handler handler_;
    };

    template<typename ConstBufferSequence, typename Handler>
    void async_write_some(const ConstBufferSequence& buf, Handler handler)
    {
      io_service_.post(write_handler<pointer, ConstBufferSequence, Handler>
                         (shared_from_this(), buf, handler));
    }

  private:
    ::BOOST_CGI_NAMESPACE::common::io_service& io_service_;
  };

 } // namespace common

  namespace connections {
    typedef common::basic_connection<common::tags::async_stdio> async_stdio;
  } // namespace connections

BOOST_CGI_NAMESPACE_END

#endif // CGI_ASYNC_STDIO_HPP_INCLUDED__
