//                 -- fcgi/client.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_CLIENT_HPP_INCLUDED__
#define CGI_FCGI_CLIENT_HPP_INCLUDED__

#include <vector>
///////////////////////////////////////////////////////////
#include <boost/shared_ptr.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/asio/buffer.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/basic_client.hpp"
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/fcgi/traits.hpp"
#include "boost/cgi/connections/shareable_tcp_socket.hpp"
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/fcgi/specification.hpp"
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/error.hpp"
#include "boost/cgi/import/read.hpp"
#include "boost/cgi/import/buffer.hpp"
#include "boost/cgi/import/io_service.hpp"

#undef min
#undef max
#include <algorithm>

#if !defined(BOOST_CGI_NO_LOGGING) && !defined(NDEBUG)
#   include <iostream>
#endif

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// A client that uses a TCP socket that owned by it.
  /// Construct
  template<>
  basic_client<
      ::BOOST_CGI_NAMESPACE::common::tags::fcgi
  >::basic_client()
    : request_id_(-1)
    , status_(none_)
    , total_sent_bytes_(0)
    , total_sent_packets_(0)
    , header_()
    , outbuf_()
    , keep_connection_(false)
  {
  }

  /// Override basic_client::close().
  /**
   * Closing a FastCGI means sending an END_REQUEST header
   * to the HTTP server and potentially closing the connection.
   *
   * Note that in general the HTTP server is responsible for the
   * lifetime of the connection, but can hand that control over
   * to the library (eg. if the server is set up to recycle
   * connections after N requests).
   */
  template<>
  boost::system::error_code
  basic_client<
      ::BOOST_CGI_NAMESPACE::common::tags::fcgi
  >::close(boost::uint64_t app_status, boost::system::error_code& ec)
  {
    // Note that the request may already be closed if the client aborts
    // the connection.
    if (!is_open())
      ec = error::already_closed;
    else
    {
      if ((constructed < status_) && (status_ < end_request_sent))
      {
        status_ = closed_;

        // Write an EndRequest packet to the server.
        outbuf_.clear();
        header_.reset(fcgi::spec_detail::END_REQUEST, request_id_, 8);

        fcgi::spec::end_request_body body(app_status, fcgi::spec_detail::REQUEST_COMPLETE);
        outbuf_.push_back(header_.data());
        outbuf_.push_back(body.data());
        write(*connection_, outbuf_, boost::asio::transfer_all(), ec);
      }
      status_ = closed_;

      if (!ec && !keep_connection_)
        connection_->close();
    }
    return ec;
  }

  /// Prepare a buffer by wrapping it into a FastCGI packet.
  /**
   * FastCGI dictates that data is sent in packets which identify
   * which request the data relates to, along with the size of
   * the packet.
   *
   * This function takes a buffer of data and creates another
   * buffer which includes the packet header information. As the
   * buffers themselves only contain pointers to the data the
   * overhead is negligible as the data is not copied.
   *
   * The lifetime of the header is guaranteed as it is kept in
   * the client object itself.
   */
  template<>
  template<typename ConstBufferSequence>
  void
  basic_client<
      ::BOOST_CGI_NAMESPACE::common::tags::fcgi
  >::prepare_buffer(const ConstBufferSequence& buf)
  {
    typename ConstBufferSequence::const_iterator iter = buf.begin();
    typename ConstBufferSequence::const_iterator end  = buf.end(); 

    outbuf_.clear();
    outbuf_.push_back(boost::asio::buffer(header_.data()));

    std::size_t total_buffer_size(0);
    for(; iter != end; ++iter)
    {
      boost::asio::const_buffer buffer(*iter);
      std::size_t new_buf_size( boost::asio::buffer_size(*iter) );
      if (total_buffer_size + new_buf_size 
           > static_cast<std::size_t>(fcgi::spec::max_packet_size::value))
      {
        // If the send buffer is empty, extract a chunk of the
        // new buffer to send. If there is already some data
        // ready to send, don't add any more data to the pack.
        if (total_buffer_size == 0)
        {
          total_buffer_size
            = std::min<std::size_t>(new_buf_size,65500);
          /*
          std::cerr<< "Oversized buffer: " << total_buffer_size
                   << " / " << new_buf_size << " bytes sent\n";
          */
          outbuf_.push_back(
            boost::asio::buffer(*iter, total_buffer_size));
          break;
        }
        else
          break;
      }
      else
      {
        total_buffer_size += new_buf_size;
        outbuf_.push_back(*iter);
      }
    }
    header_.reset(fcgi::spec_detail::STDOUT, request_id_, total_buffer_size);
  }
  
  template<>
  void
  basic_client<
      ::BOOST_CGI_NAMESPACE::common::tags::fcgi
  >::handle_write(std::size_t bytes_transferred, boost::system::error_code& ec)
  {
    total_sent_bytes_ += bytes_transferred;
    total_sent_packets_ += 1;
    
    std::size_t total_buffer_size = static_cast<std::size_t>(header_.content_length());
    
#if !defined(BOOST_CGI_NO_LOGGING) && !defined(NDEBUG)
    std::ofstream log("../logs/fcgi_client.log", std::ios::out | std::ios::app);
    if (ec)
      std::cerr<< "Error " << ec << ": " << ec.message() << '\n';
    else    
      //std::cerr
      log
        << "Transferred " << total_buffer_size
        << " (+" << (bytes_transferred - total_buffer_size)
        << " protocol) bytes (running total: "
        << total_sent_bytes_ << " bytes; "
        << total_sent_packets_ << " packets).\n";
#endif // !defined(BOOST_CGI_NO_LOGGING) && !defined(NDEBUG)

    // Now remove the protocol overhead for the caller, who
    // doesn't want to count it.
    bytes_transferred -= fcgi::spec::header_length::value;
    // Check everything was written ok.
    if (!ec && bytes_transferred != total_buffer_size)
      ec = ::BOOST_CGI_NAMESPACE::fcgi::error::couldnt_write_complete_packet;
  }


  /// Write some data to the client.
  /**
   * Currently this actually writes an entire packet - which may be up
   * to 65,535 bytes in size - as the connection may potentially be
   * multiplexed and shared between requests.
   */
  template<>
  template<typename ConstBufferSequence>
  std::size_t 
  basic_client<
      ::BOOST_CGI_NAMESPACE::common::tags::fcgi
  >::write_some(
      const ConstBufferSequence& buf
    , boost::system::error_code& ec
  )
  {
    prepare_buffer(buf);
    
    std::size_t bytes_transferred
      = boost::asio::write(*connection_, outbuf_
                          , boost::asio::transfer_all(), ec);

    handle_write(bytes_transferred, ec);
    
    return bytes_transferred;
  }


  /// Write some data to the client.
  /**
   * Currently this actually writes an entire packet - which may be up
   * to 65,535 bytes in size - as the connection may potentially be
   * multiplexed and shared between requests.
   */
  template<>
  template<typename ConstBufferSequence, typename Handler>
  void
  basic_client<
    ::BOOST_CGI_NAMESPACE::common::tags::fcgi
  >::async_write_some(
      const ConstBufferSequence& buf
    , Handler handler
  )
  {
    prepare_buffer(buf);
    boost::system::error_code ec;
    std::size_t bytes_transferred
      = boost::asio::write(*connection_, outbuf_
                          , boost::asio::transfer_all(), ec);
    handle_write(bytes_transferred, ec);
    handler(bytes_transferred, ec);
  }

 } // namespace common

namespace fcgi {
    typedef
      common::basic_client<
        ::BOOST_CGI_NAMESPACE::common::tags::fcgi
      >
    client;
} // namespace fcgi

BOOST_CGI_NAMESPACE_END

#endif // CGI_FCGI_CLIENT_HPP_INCLUDED__

