//          -- fcgi/acceptor_service_impl.hpp --
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_FCGI_TRANSPORT_SELECTION_HPP_INCLUDED__
#define CGI_FCGI_TRANSPORT_SELECTION_HPP_INCLUDED__



#include "boost/cgi/detail/push_options.hpp"

#include <algorithm>
#include <limits>
#include <boost/asio.hpp>

BOOST_CGI_NAMESPACE_BEGIN

  namespace detail {
    struct transport {
      enum type {
        pipe = 0,
        socket
      };
    };


#if    BOOST_WINDOWS
    inline transport::type transport_type() {
      HANDLE hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
      if (hStdInput != INVALID_HANDLE_VALUE) 
      {
        SOCKET socket = reinterpret_cast<SOCKET&>(hStdInput);
        SOCKADDR_STORAGE sockStor;
        int addrInfoSize = sizeof(sockStor);
        ::ZeroMemory(&sockStor, sizeof(sockStor));
        if (::getsockopt(socket, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char*>(&sockStor), &addrInfoSize) == 0)
          return transport::socket;
        else
          return transport::pipe;
      }

      return transport::pipe;
    }

    inline boost::asio::ip::tcp::socket::native_handle_type socket_handle(boost::system::error_code &ec = boost::system::error_code())
    {
        HANDLE std_input = ::GetStdHandle(STD_INPUT_HANDLE);
        SOCKET socket = 0;
        static_assert(sizeof(SOCKET) == sizeof(HANDLE), "incompatible data type sizes for reinterpret_cast");

        WSAPROTOCOL_INFO pi;
        static_assert(sizeof(pi) % sizeof(DWORD) == 0, "invalid alignment for raw pointer in std::fill_n");
        std::fill_n(static_cast<DWORD*>(static_cast<void*>(&pi)), sizeof(pi) / sizeof(DWORD), 0);
        static_assert(sizeof(HANDLE) == sizeof(SOCKET), "incompatible data type sizes for reinterpret_cast");
        if (::WSADuplicateSocket(reinterpret_cast<SOCKET>(std_input), ::GetCurrentProcessId(), &pi) != 0)
            ec = boost::system::error_code(::WSAGetLastError(), boost::system::system_category());
        else if ((socket = ::WSASocket(pi.iAddressFamily, pi.iSocketType, pi.iProtocol, &pi, 0, 0)) == (std::numeric_limits<SOCKET>::max)())
            ec = boost::system::error_code(::WSAGetLastError(), boost::system::system_category());
        else if (::SetStdHandle(STD_INPUT_HANDLE, reinterpret_cast<HANDLE>(socket)) == FALSE)
            ec = boost::system::error_code(::GetLastError(), boost::system::system_category());
        else if (::closesocket(reinterpret_cast<SOCKET>(std_input)) == SOCKET_ERROR)
            ec = boost::system::error_code(::WSAGetLastError(), boost::system::system_category());

        return socket;
    }

    inline HANDLE stream_handle(boost::system::error_code &ec = boost::system::error_code())
    {
        HANDLE stdin_handle = ::GetStdHandle(STD_INPUT_HANDLE);
        HANDLE listen_handle = INVALID_HANDLE_VALUE;
        if (!::DuplicateHandle(
            ::GetCurrentProcess()
            , stdin_handle
            , ::GetCurrentProcess()
            , &listen_handle
            , 0
            , TRUE
            , DUPLICATE_SAME_ACCESS))
        {
            ec = boost::system::error_code(::GetLastError(), boost::system::system_category());
        }
        else if (::SetStdHandle(STD_INPUT_HANDLE, listen_handle) == FALSE)
        {
            ec = boost::system::error_code(::GetLastError(), boost::system::system_category());
        }
        else
        {
            ::CloseHandle(stdin_handle);

            DWORD pipe_mode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT;
            if (::SetNamedPipeHandleState(listen_handle, &pipe_mode, NULL, NULL) == FALSE)
                ec = boost::system::error_code(::GetLastError(), boost::system::system_category());
        }

        return listen_handle;
    }

    inline boost::asio::ip::tcp::endpoint detect_endpoint(SOCKET socket = (std::numeric_limits<SOCKET>::max)()) {
      if (socket == (std::numeric_limits<SOCKET>::max)())
        socket = reinterpret_cast<SOCKET>(::GetStdHandle(STD_INPUT_HANDLE));

      if (reinterpret_cast<HANDLE>(socket) != INVALID_HANDLE_VALUE)
      {
        SOCKADDR_STORAGE sockStor;
        int addrInfoSize = sizeof(sockStor);
        ::ZeroMemory(&sockStor, sizeof(sockStor));
        if (::getsockopt(socket, SOL_SOCKET, SO_BSP_STATE, reinterpret_cast<char*>(&sockStor), &addrInfoSize) == 0)
        {
          SOCKADDR *pSockAddr = nullptr;
          CSADDR_INFO *pAddrInfo = reinterpret_cast<CSADDR_INFO*>(&sockStor);
          if (pAddrInfo->LocalAddr.lpSockaddr != nullptr)
            pSockAddr = pAddrInfo->LocalAddr.lpSockaddr;
          else if (pAddrInfo->RemoteAddr.lpSockaddr != nullptr)
            pSockAddr = pAddrInfo->RemoteAddr.lpSockaddr;

          if (pSockAddr != nullptr)
          {
            switch (pSockAddr->sa_family)
            {
              case AF_INET:
              {
                sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(pSockAddr);
                boost::asio::ip::address_v4 v4(ntohl(sa->sin_addr.S_un.S_addr));
                boost::asio::ip::address addr(v4);
                return boost::asio::ip::tcp::endpoint(addr, ntohs(sa->sin_port));
              }

              case AF_INET6:
              {
                sockaddr_in6* sa = reinterpret_cast<sockaddr_in6*>(pSockAddr);
                boost::asio::ip::address_v6 v6(reinterpret_cast<boost::asio::ip::address_v6::bytes_type const&>(sa->sin6_addr.u.Byte), ntohl(sa->sin6_scope_id));
                boost::asio::ip::address addr(v6);
                return boost::asio::ip::tcp::endpoint(addr, ntohs(sa->sin6_port));
              }
            }
          }
        }
      }

      return boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0);
    }
#else
       inline transport::type transport_type() {
           return transport::socket;
       }

       inline boost::asio::ip::tcp::endpoint detect_endpoint() {
         return boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0);
       }

#endif // defined (BOOST_WINDOWS)

   } // namespace detail
BOOST_CGI_NAMESPACE_END

#include "boost/cgi/detail/pop_options.hpp"

#endif // CGI_FCGI_TRANSPORT_SELECTION_HPP_INCLUDED__
