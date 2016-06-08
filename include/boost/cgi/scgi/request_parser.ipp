//             -- scgi/request_parser.ipp --
//
//            Copyright (c) Darren Garvey 2010.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_SCGI_REQUEST_PARSER_IPP_INCLUDED__
#define CGI_SCGI_REQUEST_PARSER_IPP_INCLUDED__

#include "boost/cgi/scgi/request_parser.hpp"
#include <algorithm>
#include <cctype>
#include <boost/lexical_cast.hpp>
#include "boost/cgi/scgi/request_impl.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace scgi {

#include "boost/cgi/detail/yield.hpp" // Enable the pseudo-keywords reenter, yield and fork.

/// The "meat" of the SCGI parser.
boost::tribool request_parser::consume(request_impl& req, char c)
{
  reenter (this)
  {
    req.headers.clear();
    req.content.clear();
    content_length_ = 0;
    headers_length_ = 0;

    // Length of headers.
    while (c != ':')
    {
      // Save the number to the content until we have the whole string,
      // at which point the content can be cleared again.
      req.content.push_back(c);
      yield return boost::indeterminate;
    }
    if (req.content.empty())
      return false;

    // Skip the ":" character.
    yield return boost::indeterminate;

    try 
    {
      headers_length_ = boost::lexical_cast<std::size_t>(req.content);
      req.content.clear();
    }
    catch (boost::bad_lexical_cast&)
    {
      return false;
    }

    writing_header_name_ = true;
    req.headers.push_back(header_impl());

    // Headers.
    while (c != ',')
    {
      if (c == '\0')
      {
        if (writing_header_name_)
          writing_header_name_ = false;
        else
        {
          writing_header_name_ = true;
          // Add another empty header.
          req.headers.push_back(header_impl());
        }
      }
      else
      {
        if (writing_header_name_)
          req.headers.back().name.push_back(c);
        else
          req.headers.back().value.push_back(c);
      }

      yield return boost::indeterminate;
    }

    // Content length. The first header MUST be the Content-length header.
    std::cerr<< std::endl << "First header is: " << req.headers.front().name << std::endl;

    for(std::vector<header_impl>::iterator iter (req.headers.begin()), end (req.headers.end())
      ; iter != end; ++iter)
    {
      std::cerr<< "Header is: " << iter->name << " = " << iter->value << std::endl;
    }


    if (req.headers.front().name != "CONTENT_LENGTH")
      return false;

    try 
    {
      std::cerr<< std::endl << "Got content-length:" << req.headers.front().value << std::endl;
      content_length_ = boost::lexical_cast<std::size_t>(req.headers.front().value);
    }
    catch (boost::bad_lexical_cast&)
    {
      return false;
    }

    // Content.
    while (req.content.size() < content_length_)
    {
      yield return boost::indeterminate;
      req.content.push_back(c);
    }
  }

  return true;
}

#include "boost/cgi/detail/unyield.hpp"

 } // namespace scgi
BOOST_CGI_NAMESPACE_END

#endif // CGI_SCGI_REQUEST_PARSER_IPP_INCLUDED__
