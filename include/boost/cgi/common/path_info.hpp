
#ifndef BOOST_CGI_COMMON_PATH_INFO_HPP_INCLUDED_
#define BOOST_CGI_COMMON_PATH_INFO_HPP_INCLUDED_

#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/fwd/basic_request_fwd.hpp"
#include "boost/cgi/config.hpp"

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

  struct path_info
  {
    typedef std::string                  value_type;
    typedef value_type                   string_type;
    typedef string_type::size_type       size_type;
    typedef std::vector<value_type>      vector_type;
    typedef vector_type::iterator        iterator;
    typedef vector_type::const_iterator  const_iterator;

  private:
    void parse()
    {
      if (value[0] != '/') value.insert(value.begin(), '/');
      boost::algorithm::split(
        parts, value, boost::algorithm::is_any_of("/"));
    }
     
    string_type substr(
        string_type const& str,
        char ch,
        bool include_char = true
      )
    const
    {
      try {
        return value.substr(value.find_last_of(ch));
      } catch(...) {
        return "";
      }
    }

  public:
     
    template<typename P>
    path_info(basic_request<P> & request)
      : value(request.env["path_info"])
    {
      parse();
    }

    path_info(value_type const& str)
      : value(str)
    {
      parse();
    }
     
    value_type& operator[](int i) { return parts[i]; }
     
    value_type& string() { return value; }
     
    operator value_type& () { return value; }
     
    string_type stem() const { return substr(value, '/', false); }
    
    string_type extension() const { return substr(stem(), '.'); }
     
    iterator begin() { return parts.begin(); }
    iterator end() { return parts.end(); }
    const_iterator begin() const { return parts.begin(); }
    const_iterator end() const { return parts.end(); }

    value_type  value;
    vector_type parts;
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_COMMON_PATH_INFO_HPP_INCLUDED_

