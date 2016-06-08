//       -- connections/shareable_tcp_socket.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_CONNECTIONS_SHAREABLE_TCP_SOCKET_HPP_INCLUDED__
#define CGI_CONNECTIONS_SHAREABLE_TCP_SOCKET_HPP_INCLUDED__

#include <map>
#include <set>
///////////////////////////////////////////////////////////
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/error.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/basic_connection.hpp"
#include "boost/cgi/import/io_service.hpp"
#include "boost/cgi/detail/push_options.hpp"
#include "boost/cgi/common/connection_base.hpp"
#include "boost/cgi/fcgi/traits.hpp"
#include "boost/cgi/common/protocol_traits.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /*** 05.02.2008 :
   *  I'm planning on making this class a more FastCGI-specific one since a
   *  rereading of the FastCGI spec (ie. 'Managing request IDs') made me
   *  realise that IDs are connection specific, not just application specific.
   *  
   *  IOW when checking to see if a particular request id exists, rather than
   *  having to check a table in the protocol_service (which may imply explicit
   *  mutex use), we can just check a local array of values. Good good.
   ***/

  template<>
  class basic_connection<tags::shareable_tcp_socket>
    : public connection_base
  {
  public:
    typedef boost::shared_ptr<
      basic_connection<tags::shareable_tcp_socket> >  pointer;
    typedef boost::mutex                              mutex_type;
    struct condition_type : public boost::condition_variable
        { typedef boost::shared_ptr<boost::condition_variable> pointer; };
  	typedef boost::mutex::scoped_lock                 scoped_lock_type;
    typedef boost::asio::ip::tcp::socket              next_layer_type;

    typedef common::protocol_traits<tags::fcgi>       traits;
    /** FastCGI specific stuff **/
    typedef traits::request_type                      request_type;
    typedef boost::shared_ptr<request_type>           request_ptr;
    typedef std::map<boost::uint16_t, request_type*>  request_map_type;
    typedef std::vector<request_type*>                request_vector_type;
    /** End FastCGI stuff      **/

    // A wrapper to provide condition_type::pointer
    basic_connection(io_service& ios)
      : sock_(ios)
      , mutex_()
      , condition_()
    {
    }

    ~basic_connection()
    {
      //BOOST_FOREACH(boost::uint16_t id, deletable_request_ids_)
      //{
      //  delete requests_.at(id-1);
      //  requests_.at(id-1) = 0;
      //}
    }

    bool is_open() const
    {
      return sock_.is_open();
    }

    void close()
    {
      sock_.close();
    }

    void lock()
    {
      scoped_lock_type lock(mutex_);
      locked_ = true;
    }

    void unlock()
    {
      scoped_lock_type lock(mutex_);
      locked_ = false;
      condition_.notify_one();
    }

    void wait()
    {
      scoped_lock_type lock(mutex_);
      if (locked_ == false)
        return;
      condition_.wait(lock);
    }

    static pointer create(io_service& ios)
    {
      return //static_cast<pointer>(
        pointer(new basic_connection<tags::shareable_tcp_socket>(ios));
    }      

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buf)
    {
      return sock_.read_some(buf);
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence& buf
                         , boost::system::error_code& ec)
    {
      return sock_.read_some(buf, ec);
    }

    template<typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buf, Handler handler)
    {
      sock_.async_read_some(buf, handler);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf)
    {
      return sock_.write_some(buf);
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence& buf
                          , boost::system::error_code& ec)
    {
      return sock_.write_some(buf, ec);
    }

    template<typename ConstBufferSequence, typename Handler>
    void async_write_some(ConstBufferSequence& buf, Handler handler)
    {
      sock_.async_write_some(buf, handler);
    }
    
    next_layer_type&
      next_layer()
    {
      return sock_;
    }

    mutex_type& mutex()         { return mutex_;     }
    condition_type& condition() { return condition_; }

    boost::system::error_code
      get_slot(boost::uint16_t id, boost::system::error_code& ec)
    {
      if (requests_.at(id-1)) // duplicate request!
      {
        return error::duplicate_request;
      }

      if (requests_.size() < boost::uint16_t(id-1))
        requests_.resize(id);

      return ec;
    }

    boost::system::error_code
      add_request(boost::uint16_t id, request_type* req, bool on_heap
                 , boost::system::error_code& ec)
    {
      requests_.at(id-1) = req;
      if (on_heap)
        deletable_request_ids_.insert(id);
      return ec;
    }

    //template<typename RequestImpl>
    //boost::system::error_code
    //  multiplex(RequestImpl& impl, boost::system::error_code& ec)
    //{
    //  
    //  return ec;
    //}
  private:
    
    bool locked_;
    next_layer_type sock_;
    mutex_type mutex_;
    condition_type condition_;

  public:
    /** FastCGI specific stuff **/
    request_map_type request_map_;
    request_vector_type requests_;
    std::set<int> deletable_request_ids_;
  };

 } // namespace common

  namespace connections {
    typedef common::basic_connection<
      common::tags::shareable_tcp_socket> shareable_tcp;
  } // namespace connections

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_CONNECTIONS_SHAREABLE_TCP_SOCKET_HPP_INCLUDED__
