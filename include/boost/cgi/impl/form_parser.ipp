//                  -- form_parser.ihpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_FORM_PARSER_IPP_INCLUDED__
#define CGI_DETAIL_FORM_PARSER_IPP_INCLUDED__

#include "boost/cgi/error.hpp"
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/detail/url_decode.hpp"
#include "boost/cgi/common/source_enums.hpp"
#include "boost/cgi/config.hpp"

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

BOOST_CGI_NAMESPACE_BEGIN

 namespace common {

    BOOST_CGI_INLINE
    boost::system::error_code
      form_parser::parse(context ctx, boost::system::error_code& ec)
    {
      context_ = &ctx;
      
      BOOST_ASSERT(!ctx.content_type.empty());

      if (ctx.content_type.find(
         "application/x-www-form-urlencoded") != string_type::npos)
        parse_url_encoded_form(ec);
      else
      if (ctx.content_type.find(
         "multipart/form-data") != string_type::npos)
        parse_multipart_form(ec);

      // NOTE: The following code prevented POST method execution for other valid mime types. (http://www.iana.org/assignments/media-types)
      //else
      //  return ec = common::error::invalid_form_type;

      return ec;
    }

    BOOST_CGI_INLINE
    boost::system::error_code
      form_parser::parse_url_encoded_form(boost::system::error_code& ec)
    {
     buffer_type& str(context_->buffer);
     string_type result;
     string_type name;

     if (str.size() == 0)
    	 return ec;

     for( buffer_type::const_iterator iter (str.begin()), end (str.end())
        ; iter != end; ++iter )
     {
       switch( *iter )
       {
         case ' ':
           break;
         case '+':
           result.append(1, ' ');
           break;
         case '%':
           if (std::distance(iter, end) <= 2
            || !std::isxdigit(*(iter+1))
            || !std::isxdigit(*(iter+2)))
           {
             result.append(1, '%');
           }
           else // we've got a properly encoded hex value.
           {
             char ch = *++iter; // need this because order of function arg
                                // evaluation is UB.
             result.append(1, detail::hex_to_char(ch, *++iter));
           }
           break;
         case '=': // the name is complete, now get the corresponding value
            name.swap(result);
            break;
         case '&': // we now have the name/value pair, so save it
            context_->data_map.insert(std::make_pair(name.c_str(), result));
            result.clear();
            name.clear();
            break;
         default:
           result.append(1, *iter);
       }
     }
#if defined(BOOST_CGI_KEEP_EMPTY_VARS)
      // save the last param (it won't have a trailing &)
      if( !name.empty() ) {
          context_->data_map.insert(std::make_pair(name.c_str(), result));
      }
#endif // BOOST_CGI_KEEP_EMPTY_VARS
     return ec;
    }

    /// Parse a multipart form.
    BOOST_CGI_INLINE
    boost::system::error_code
      form_parser::parse_multipart_form(boost::system::error_code& ec)
    {
      parse_boundary_marker(ec);
      move_to_start_of_first_part(ec);

      while(!ec && !context_->stdin_parsed)
        parse_form_part(ec);

      return ec;
    }


    BOOST_CGI_INLINE
    boost::system::error_code
      form_parser::parse_form_part(boost::system::error_code& ec)
    {
      namespace algo = boost::algorithm;

      string_type marker(
        string_type("--") + context_->boundary_markers.front());
      string_type& buffer(context_->buffer);
      std::size_t& offset (context_->offset);

      std::size_t end = buffer.find("\r\n\r\n", offset);
      if (end == string_type::npos)
        return ec = common::error::multipart_meta_data_not_terminated;
      
      string_type meta (buffer.substr(offset,end-offset));
      
      // sic - short-cut check for Content-Disposition.
      std::size_t pos1 = meta.find("isposition:"); // sic
      std::size_t pos2 = meta.find(";", pos1);
      std::size_t pos3 = meta.find("name=");
      std::size_t pos4 = meta.find(";", pos3);
      std::size_t pos5 = meta.find("\r\n");
      std::size_t pos6 = meta.find("filename=", pos2);
      
      if (pos3 == string_type::npos)
        pos3 = meta.find("\r\n");
      string_type field_name (meta.substr(pos3+5, pos4-pos3-5));
      algo::trim_if(field_name, algo::is_any_of("\" "));
      
      common::form_part part;
      part.name = field_name;
      part.content_disposition = meta.substr(pos1+11, pos2-pos1-11);

      std::size_t next_pos = buffer.find(string_type("\r\n") + marker, end);
      
      if (pos6 == string_type::npos)
      {
        string_type content (
           buffer.substr(meta.length()+4, next_pos-meta.length()-4));
        
        // Load the data to the request's post map.
        part.value = content;
      }
      else
      {
        string_type filename (meta.substr(pos6+9, pos5-pos6-9));
        algo::trim_if(filename, algo::is_any_of("\" "));
        // Load the filename as the value on the request's post map.
        //part.value = "<FileUpload: '" + filename + "'>";
        part.value = filename;
        // Empty parameters could probably be left out, but setting even
        // an empty variable is consistent with the rest of the library.
        // **FIXME** Might be useful to respect BOOST_CGI_KEEP_EMPTY_VARS
        // here. Leaving that out as it would not be expected, AFAIK.
        if (!filename.empty())
        {
#ifndef BOOST_CGI_NO_BOOST_FILESYSTEM
          part.filename = filename;
          // Load the data to a local file.
          string_type content (
              buffer.substr(meta.length()+4, next_pos-meta.length()-4));
          string_type randomatter (
            boost::lexical_cast<string_type>(time(NULL)));
          string_type user_ip (context_->random_string);
          // Clean dangerous characters.
          algo::trim_if(filename, algo::is_any_of(BOOST_CGI_UNSAFE_FILENAME_CHARS));
          string_type internal_filename(
            BOOST_CGI_UPLOAD_DIRECTORY+filename+"."+user_ip+"."+randomatter);
          part.path = internal_filename;
          std::ofstream file (
              internal_filename.c_str()
            , std::ios::out | std::ios::binary);
          file<< content;
          context_->uploads_map.insert(std::make_pair(part.name.c_str(), part));
#endif // BOOST_CGI_NO_BOOST_FILESYSTEM
        }
      }
      // Load the data to the request's post map.
      context_->data_map.insert(std::make_pair(part.name.c_str(), part.value));
      
      buffer.erase(0, next_pos+marker.length()+2);
      if (buffer.length() >= 2
        && buffer.substr(0,2) == "--")
      {
         //ec = common::error::eof;
         context_->stdin_parsed = true;
         context_->bytes_left = 0;
      }
      else
      if (buffer.length() == 0)
        context_->bytes_left = 0;
        
      buffer.erase(0,2);

      return ec;
    }

    BOOST_CGI_INLINE
    boost::system::error_code
      form_parser::move_to_start_of_first_part(boost::system::error_code& ec)
    {
      string_type marker(
        string_type("--") + context_->boundary_markers.front() + "\r\n");
      string_type& buffer(context_->buffer);

      std::size_t pos = buffer.find(marker);

      if (pos == string_type::npos)
        ec = common::error::multipart_form_boundary_not_found;
      else
        buffer.erase(0, pos+marker.length());
      
      ec = boost::system::error_code();

      return ec;
    }

    BOOST_CGI_INLINE
    boost::system::error_code
      form_parser::parse_boundary_marker(boost::system::error_code& ec)
    {
      string_type& ctype(context_->content_type);
      string_type& marker(context_->boundary_marker);
      
      marker.assign(ctype.substr(ctype.find("boundary=")+9));
      boost::algorithm::trim(marker);
      if (marker.empty())
        ec = common::error::no_boundary_marker;
      else
        context_->boundary_markers.push_front(marker);

      return ec;
    }

 } // namespace common
 
BOOST_CGI_NAMESPACE_END

#endif // CGI_DETAIL_FORM_PARSER_IPP_INCLUDED__

