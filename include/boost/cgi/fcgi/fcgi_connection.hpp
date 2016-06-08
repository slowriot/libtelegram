//               -- anonymous_pipe.hpp --
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef BOOST_CGI_WIN32_CONNECTION_INCLUDED__
#define BOOST_CGI_WIN32_CONNECTION_INCLUDED__

#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/connections/anonymous_pipe.hpp"
#include "boost/cgi/connections/shareable_tcp_socket.hpp"
#include "boost/cgi/fcgi/transport_selection.hpp"

//////////////////////////////////////////////////////////////////////////

BOOST_CGI_NAMESPACE_BEGIN
  namespace common {

  /// Specialisation of basic_connection for fcgi on Windows.

  template<>
  class basic_connection<tags::fcgi_transport>
    : public connection_base
  {
  public:
    typedef basic_connection<tags::fcgi_transport> type;
    typedef boost::shared_ptr<type>                pointer;

    basic_connection(io_service& ios)
      : transport_(detail::transport_type())
    {
      if (transport_ == detail::transport::pipe)
#if BOOST_WINDOWS
        // pipe_.reset(new boost::asio::windows::stream_handle(ios));
        pipe_.reset(new basic_connection<tags::anonymous_pipe>(ios));
#else
        pipe_.reset(new boost::asio::local::stream_protocol::socket(ios));
#endif // BOOST_WINDOWS
      else // transport_ == detail::transport::socket
        socket_.reset(new boost::asio::ip::tcp::socket(ios));
    }

    bool is_open() const
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->is_open();
      else // transport_ == detail::transport::socket
        return socket_->is_open();
    }

    void close()
    {
      if (transport_ == detail::transport::pipe)
        pipe_->close();
      else // transport_ == detail::transport::socket
        socket_->close();
    }

    static pointer create(io_service& ios)
    {
        return pointer( new basic_connection<tags::fcgi_transport>(ios));
    }      

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buf)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->read_some(buf);
      else // transport_ == detail::transport::socket
        return socket_->read_some(buf);
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buf
                         , boost::system::error_code& ec)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->read_some(buf, ec);
      else // transport_ == detail::transport::socket
        return socket_->read_some(buf, ec);
    }

    template<typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buf, Handler handler)
    {
      if (transport_ == detail::transport::pipe)
        pipe_->async_read_some(buf, handler);
      else // transport_ == detail::transport::socket
        socket_->async_read_some(buf, handler);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->write_some(buf);
      else // transport_ == detail::transport::socket
        return socket_->write_some(buf);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      if (transport_ == detail::transport::pipe)
        return pipe_->write_some(buf, ec);
      else // transport_ == detail::transport::socket
        return socket_->write_some(buf, ec);
    }

    template<typename ConstBufferSequence, typename Handler>
    void async_write_some(ConstBufferSequence& buf, Handler handler)
    {
      if (transport_ == detail::transport::pipe)
        pipe_->async_write_some(buf, handler);
      else // transport_ == detail::transport::socket
        socket_->async_write_some(buf, handler);
    }

  public:
#if BOOST_WINDOWS
    // boost::scoped_ptr<boost::asio::windows::stream_handle> pipe_;
    // NOTE: windows::stream_handle has the precondition that the handle was opened with FILE_FLAG_OVERLAPPED.
    //       Apache's mod_fcgid (and others?) create the named pipe handle without this before passing it as
    //       stdin.  Due to the way that windows pipe client/server semantics work it is impossiblet to upgrade
    //       the handle access using ReOpenFile() since the result is ERROR_PIPE_BUSY ... indefinately due to
    //       blocking nature of the current handle state...  in short, not compatible with windows::stream_handle.
    boost::scoped_ptr<basic_connection<tags::anonymous_pipe>> pipe_;
    
#else
    boost::scoped_ptr<boost::asio::local::stream_protocol::socket> pipe_;
#endif // BOOST_WINDOWS
    boost::scoped_ptr<boost::asio::ip::tcp::socket> socket_;
  private:

    detail::transport::type transport_;
  };
  
 } // namespace common

  namespace connections {
    typedef common::basic_connection<common::tags::fcgi_transport> fcgi_transport;
  } // namespace connections

BOOST_CGI_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////

#include "boost/cgi/detail/pop_options.hpp"

#endif // BOOST_CGI_WIN32_CONNECTION_INCLUDED__
