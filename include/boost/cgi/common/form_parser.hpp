//                   -- form_parser.hpp --
//
//            Copyright (c) Darren Garvey 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
////////////////////////////////////////////////////////////////
#ifndef CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__
#define CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__

#include "boost/cgi/detail/push_options.hpp"

///////////////////////////////////////////////////////////
#include <set>
#include <vector>
#include <string>
///////////////////////////////////////////////////////////
#include <boost/regex.hpp>
#include <boost/function.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/algorithm/string/find.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/common/map.hpp"
#include "boost/cgi/common/form_part.hpp"
#include "boost/cgi/config.hpp"

/// Set the directory that uploads are stored in.
/**
 * When files are uploaded, using multipart/form-data, uploaded
 * files are saved to disk, rather than being held in memory.
 *
 * This macro determines where uploaded files are stored. It must
 * include a trailing slash (eg. "../upload/", which is the default).
 *
 * Your web server must run as a user that has read and write
 * permissions to this directory. You should never allow this to
 * be the same directory the FastCGI script is running in, or any
 * directory containing executable files.
 */
#if !defined(BOOST_CGI_UPLOAD_DIRECTORY)
#   define BOOST_CGI_UPLOAD_DIRECTORY "../uploads/"
#endif // BOOST_CGI_UPLOAD_DIRECTORY

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  /// A class for parsing POST data sent to a CGI process.
  /**
   * Construct this and then call `form_parser::parse` with an
   * instance of form_parser::context (or compatible struct).
   *
   * This is minimal and doesn't extract all meta-data yet, but
   * is known to work on Windows XP with MSVC9.0 and Ubuntu linux
   * with gcc 4.2.x and 4.3.x.
   *
   * Valid Form Encodings
   * > `application/x-www-form-urlencoded`
   * > `multipart/form-data`
   *
   * File uploads (ie. in `multipart/form-data` forms) are saved
   * to disk. See the `BOOST_CGI_UPLOAD_DIRECTORY` macro.
   *
   * Should also work for HTTP POST data.
   * 
   */
  class form_parser
  {
  public:
    /// The callback functor to read more data.
    typedef
      boost::function<
        std::size_t (boost::system::error_code&)
      >
    callback_type;

    typedef common::map                     map_type;
    typedef common::form_part::string_type  string_type;
    typedef common::form_part::buffer_type  buffer_type;
    typedef boost::asio::mutable_buffers_1  mutable_buffers_type;

    /// The context used for parsing.
    struct context
    {
      string_type& content_type;
      buffer_type& buffer;
      std::size_t& bytes_left;
      common::post_map& data_map;
      common::upload_map& uploads_map;
      const callback_type callback;
      bool& stdin_parsed;
      // A random string, used for marking uploaded files.
      // Set this to something like the user's REMOTE_ADDR.
      string_type random_string;
      std::size_t offset;

      buffer_type::iterator pos;
      string_type boundary_marker;
      std::list<string_type> boundary_markers;
    };
    
    form_parser() : context_(NULL) {}
/*
    form_parser(
        buffer_type&, common::post_map&, string_type const&
      , callback_type const&, std::size_t&, bool&); 
*/

    string_type buffer_string()
    {
      return string_type(context_->buffer.begin() + context_->offset
                        , context_->buffer.end());
    }
    
    /// Run the parser on the given `context`.
    boost::system::error_code 
      parse(context ctx, boost::system::error_code& ec);

    /// URL-encoded forms.
    /**
     * Parse forms where the content-type is "application/www-url-encoded".
     */
    boost::system::error_code
      parse_url_encoded_form(boost::system::error_code& ec);

    /// Parse a multipart form.
    /**
     * Parse forms where the content-type is "multipart/form-data".
     */
    boost::system::error_code
      parse_multipart_form(boost::system::error_code& ec);

    /// Parse a single form part.
    boost::system::error_code
      parse_form_part(boost::system::error_code& ec);

    /// Erase any front-cruft on the form data.
    /**
     * In multipart forms, any characters that precede the first form
     * boundary are ignored. This function erases those characters.
     */
    boost::system::error_code
      move_to_start_of_first_part(boost::system::error_code& ec);

    /// Get the boundary marker from the CONTENT_TYPE header.
    boost::system::error_code
      parse_boundary_marker(boost::system::error_code& ec);

private:
    context* context_;
  };

 } // namespace common

BOOST_CGI_NAMESPACE_END

//#ifndef BOOST_CGI_BUILD_LIB
#    include "boost/cgi/impl/form_parser.ipp"
//#endif

#endif // CGI_DETAIL_FORM_PARSER_HPP_INCLUDED__

