//                 -- basic_client.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_BASIC_CLIENT_HPP_INCLUDED__
#define CGI_BASIC_CLIENT_HPP_INCLUDED__

#include <boost/shared_ptr.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/common/role_type.hpp"
#include "boost/cgi/common/request_status.hpp"
#if defined(BOOST_WINDOWS)
#  include "boost/cgi/fcgi/fcgi_connection.hpp"
#else
#  include "boost/cgi/connections/tcp_socket.hpp"
#endif // defined(BOOST_WINDOWS)
#include "boost/cgi/detail/throw_error.hpp"
#include "boost/cgi/error.hpp"
#include "boost/cgi/http/status_code.hpp"


BOOST_CGI_NAMESPACE_BEGIN
 namespace common {
 
  enum client_status
  {
    none_, // **FIXME** !
    constructed,
    params_read,
    stdin_read,
    end_request_sent,
    closed_, // **FIXME** !
    //aborted
  };

  /// A client
  /**
   * A client is for two things:
   * 1. To hold a full-duplex connection (or separate input and output
   *    connections).
   * 2. To hold any protocol-specific data about the request. For now,
   *    this means the internal 'request number' associated by FastCGI
   *    with each request (ie. so incoming/outgoing packets can be wrapped
   *    with data noting what request it relates to).
   * 3. Buffering. Not sure about how far this should go yet, but probably
   *    no further than minimal buffering.
   * 4. Share a connection. Since a multiplexing connection is shared between
   *    multiple clients, the client should be responsible for taking possesion
   *    of the connection for a period of time (so it can write a complete
   *    packet). This idea could be taken quite far into genericity by making
   *    clients aware of how busy the connection is and size its output packets
   *    accordingly... But I'm not doing that.
   */
  template<typename Protocol>
  class basic_client
  {
  public:
    typedef Protocol                              protocol_type;
    typedef common::map                           map_type;
    typedef protocol_traits<Protocol>             traits;
    typedef typename traits::connection_type      connection_type;
    typedef typename traits::mutable_buffers_type mutable_buffers_type;
    typedef typename traits::header_buffer_type   header_buffer_type;
    typedef typename connection_type::pointer     connection_ptr;

    basic_client()
    {
    }

    /// Construct the client by claiming a request id.
    /**
     * Before loading a request, it will usually not have a request id. This
     * function reads headers (and corresponding bodies if necessary) until
     * a BEGIN_REQUEST record is found. The calling request then claims and
     * serves that request.
     */
    template<typename RequestImpl>
    boost::system::error_code
      construct(RequestImpl& req, boost::system::error_code& ec)
    {
      status_ = constructed;
      return ec;
    }
    
    boost::system::error_code
      close(boost::uint64_t app_status, boost::system::error_code& ec)
    {
      if (!is_open())
        ec = error::already_closed;
      else
      {
        status_ = closed_;
        connection_->close();
      }
      return ec;
    }

    void close(boost::uint64_t app_status = 0)
    {
      boost::system::error_code ec;
      close(app_status, ec);
      detail::throw_error(ec);
    }

    /// Associate a connection with this client
    /**
     * Note: the connection must have been created using the new operator
     */
    bool set_connection(connection_type* conn)
    {
      connection_.reset(conn);
      return true;
    }

    //io_service& io_service() { return io_service_; }

    /// Associate a connection with this client
    /**
     * Note: the connection must have been created using the new operator
     */
    bool set_connection(const typename connection_type::pointer& conn)
    {
      connection_  = conn;
      return true;
    }

    /// Write some data to the client.
    template<typename ConstBufferSequence>
    std::size_t write_some(const ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      return connection_->write_some(buf, ec);
    }

    /// Read data into the supplied buffer.
    /**
     * Reads some data that, correctly checking and stripping FastCGI headers.
     *
     * Returns the number of bytes read and sets `ec` such that `ec` evaluates
     * to `true` iff an error occured during the read operation.
     *
     * Notable errors:
     * - `fcgi::error::data_for_another_request`
     * - `fcgi::error::connection_locked`
     *
     * These must be dealt with by user code if they choose to read through the
     * client (reading through the request is recommended).
     */
    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buf
                         , boost::system::error_code& ec)
    {
      std::size_t bytes_read = connection_->read_some(buf, ec);
      bytes_left_ -= bytes_read;
      if (ec == boost::asio::error::eof)
        ec = boost::system::error_code();
      return bytes_left_ > 0 ? bytes_read : (bytes_read + bytes_left_);
    }

    /// Asynchronously write some data to the client.
    template<typename ConstBufferSequence, typename Handler>
    void async_write_some(const ConstBufferSequence& buf, Handler handler)
    {
      connection_->async_write_some(buf, handler);
    }

    /// Asynchronously read some data from the client.
    template<typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buf, Handler handler)
    {
      connection_->async_read_some(buf, handler);
    }

    ////// Querying the client.

    /// Get a shared_ptr of the connection associated with the client.
    connection_ptr& connection() { return connection_; }
    std::size_t& bytes_left()    { return bytes_left_; }
    
    bool is_open()
    {
      return connection_->is_open();
    }

    /// Get the status of the client.
    const client_status& status() const
    {
      return status_;
    }

    /// Set the status of the client.
    void status(client_status status)
    {
      status_ = status;
    }

    bool keep_connection() const
    {
      return keep_connection_;
    }
    
    boost::uint16_t const& request_id() const
    {
      return request_id_;
    }

  private:
  
    template<typename ConstBufferSequence>
    void prepare_buffer(const ConstBufferSequence& buf)
    { /* NOOP */ }
        
    void handle_write(
      std::size_t bytes_transferred, boost::system::error_code& ec)
    { /* NOOP */ }
  
    //io_service&                           io_service_;
    connection_ptr                        connection_;

  public: // **FIXME**
    // we should never read more than content-length bytes.
    std::size_t bytes_left_;
    
    boost::uint16_t request_id_;
    client_status status_;
    
    boost::uint64_t total_sent_bytes_;
    boost::uint64_t total_sent_packets_;

    /// Buffer used to check the header of each packet.
    fcgi::spec::header header_;

    /// Output buffer.
    /**
     * This doesn't take ownership of the underlying memory, so the
     * data must remain valid until it has been completely written.
     */
    std::vector<boost::asio::const_buffer> outbuf_;

    bool keep_connection_;
    common::role_type role_;
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // CGI_BASIC_CLIENT_HPP_INCLUDED__
