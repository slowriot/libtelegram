//                     -- stdio.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_STDIO_CONNECTION_IMPL_HPP_INCLUDED__
#define CGI_STDIO_CONNECTION_IMPL_HPP_INCLUDED__

#include <cstdio>
#include <string>
#include <boost/config.hpp>
#if BOOST_WINDOWS
#   include <stdio.h> // for _setmode
#   include <io.h>    // for _setmode
#   include <fcntl.h> // for _setmode
#endif // BOOST_WINDOWS
///////////////////////////////////////////////////////////
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/error.hpp"
#include "boost/cgi/common/tags.hpp"
#include "boost/cgi/basic_connection.hpp"
#include "boost/cgi/detail/push_options.hpp"
#include "boost/cgi/common/connection_base.hpp"
#include "boost/cgi/fwd/basic_connection_fwd.hpp"
//#include "boost/cgi/common/protocol_traits.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  template<>
  class basic_connection<tags::stdio>
    : public connection_base
  {
  public:
    typedef boost::shared_ptr<basic_connection<tags::stdio> >  pointer;

    basic_connection()
      : is_open_(true)
    {
#if BOOST_WINDOWS
      // We have to open stdin in binary mode on windows,
      // or "\r\n" sequences are truncated to "\n".
      _setmode(_fileno(stdin),_O_BINARY);
      _setmode(_fileno(stdout),_O_BINARY);
      _setmode(_fileno(stderr),_O_BINARY);
#endif
    }

    template<typename T>
    basic_connection(T&)
      : is_open_(true)
    {
#if BOOST_WINDOWS
      // We have to open stdin in binary mode on windows,
      // or "\r\n" sequences are truncated to "\n".
      _setmode(_fileno(stdin),_O_BINARY);
      _setmode(_fileno(stdout),_O_BINARY);
      _setmode(_fileno(stderr),_O_BINARY);
#endif
    }

    bool is_open() const
    {
      return is_open_;
    }

    void close()
    {
      is_open_ = false;
    }

    static pointer create()
    {
      return pointer(new basic_connection<tags::stdio>());
    }

    template<typename MutableBufferSequence>
    std::size_t read_some(const MutableBufferSequence buf
                         , boost::system::error_code& ec)
    {
      if (std::fread(boost::asio::buffer_cast<void *>(buf)
                    , boost::asio::buffer_size(buf)
                    , 1, stdin))
      {
        return strlen(boost::asio::buffer_cast<char *>(buf));
      }

      if (std::feof(stdin))
        ec = ::BOOST_CGI_NAMESPACE::error::eof;
      else
      if (std::ferror(stdin))
        ec = ::BOOST_CGI_NAMESPACE::error::bad_read;
      else
        ec = ::BOOST_CGI_NAMESPACE::error::broken_pipe;

      return 0;
    }

    template<typename ConstBufferSequence>
    std::size_t write_some(ConstBufferSequence const& buf
                          , boost::system::error_code& ec)
    {
      ec = boost::system::error_code();
      
      std::size_t bytes_transferred(0);
      for(typename ConstBufferSequence::const_iterator i = buf.begin(),
          end (buf.end())
         ; !ec && i != end; ++i)
      {
        std::size_t buf_len = boost::asio::buffer_size(*i);
        bytes_transferred += buf_len;
        //int ret(fputs(boost::asio::buffer_cast<const char*>(*i), stdout));
        //if (ret == EOF)
        //{
        //  return ::BOOST_CGI_NAMESPACE::error::broken_pipe;
        //}
        //std::cerr<< "[buf] " 
        // << std::string(boost::asio::buffer_cast<const char*>(*i), buf_len)
        // << std::endl;
        if (!std::fwrite(boost::asio::buffer_cast<const void *>(*i)
                       , buf_len, 1, stdout))
        {
          if (std::feof(stdout))
            ec = ::BOOST_CGI_NAMESPACE::error::eof;
          else
          if (std::ferror(stdout))
            ec = ::BOOST_CGI_NAMESPACE::error::bad_write;
          else
            ec = ::BOOST_CGI_NAMESPACE::error::broken_pipe;
        }
      }
      return bytes_transferred;
    }

  protected:
    bool is_open_;
  };

 } // namespace common

  namespace connections {
    typedef common::basic_connection<common::tags::stdio> stdio;
  } // namespace connections

BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_STDIO_CONNECTION_IMPL_HPP_INCLUDED__
