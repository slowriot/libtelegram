#ifndef BOOST_CGI_FORM_PART_HPP_INCLUDED__
#define BOOST_CGI_FORM_PART_HPP_INCLUDED__

#include <map>
#include <vector>
#include <string>
#include <boost/range.hpp>
#ifndef BOOST_CGI_NO_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
#endif // BOOST_CGI_NO_BOOST_FILESYSTEM
#include "boost/cgi/config.hpp"
#include "boost/cgi/common/name.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

   struct form_part
   {
     typedef char                          char_type;
     typedef std::basic_string<char_type>  string_type;
     typedef string_type                   buffer_type;
     typedef buffer_type::iterator         iter_t;
     
     typedef boost::iterator_range<
       buffer_type::const_iterator
     > range_type;

     typedef std::pair<iter_t, iter_t> pair_t;
     
     typedef std::map<
       string_type, pair_t
     > meta_data_map_type;
     
     form_part() {}
     form_part(string_type const& val) : value(val) {}
     form_part(const char_type* val) : value(val) {}
     
     ~form_part()
     {
#ifndef BOOST_CGI_KEEP_FILE_UPLOADS
#ifndef BOOST_CGI_NO_BOOST_FILESYSTEM
       try {
           // Delete any file saved in uploading this.
           namespace fs = boost::filesystem;
           if (!path.empty() && fs::exists(path)) {
             fs::remove(path);
           }
       } catch(...) {
           // pass
       }
#endif // BOOST_CGI_NO_BOOST_FILESYSTEM
#endif // BOOST_CGI_KEEP_FILE_UPLOADS
     }

     meta_data_map_type meta_data_;

     /// The boundary marker that's needed.
     // If this is empty, it means the corresponding data has been read.
     string_type boundary_marker_;

     //range_type buffer_;
     pair_t buffer_;

     string_type content_type; // must exist
     string_type content_disposition; // must exist
     string_type name; // must exist
     string_type value; // Either the data itself, or the filename for file uploads.
#ifndef BOOST_CGI_NO_BOOST_FILESYSTEM
     boost::filesystem::path filename;
     // Where the actual uploaded file is stored.
     boost::filesystem::path path;
#endif // BOOST_CGI_NO_BOOST_FILESYSTEM

   public:
     /// TODO: Check that the uploaded file isn't empty too.
     bool empty() const { return value.empty(); }
    
     const char_type* c_str() const { return value.c_str(); }
     operator const char_type* () const { return value.c_str(); }
     operator string_type const& () const { return value; }
     operator string_type () const { return value; }
     template<typename T>
     operator std::basic_string<T> () { return value; }
    
     friend std::ostream& operator<<(std::ostream& os, form_part const& part);
   };

  inline
  std::pair<common::name, common::form_part>
    make_pair (const char* n, common::form_part& part)
  {
    part.value.swap(part.name);
    part.name = n;
    return std::make_pair(n, part);
  }

  inline
  std::pair<common::name, common::form_part>
    make_pair (const char* n, const char* v)
  {
    common::form_part part;
    part.name = n;
    part.value = v;
    return std::make_pair(n, part);
  }

  inline std::ostream& operator<< (std::ostream& os, BOOST_CGI_NAMESPACE::common::form_part const& part)
  {
    os<< part.value;
    return os;
  }

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_FORM_PART_HPP_INCLUDED__

