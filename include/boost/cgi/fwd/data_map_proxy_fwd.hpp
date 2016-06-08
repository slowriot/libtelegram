
#ifndef BOOST_CGI_DATA_MAP_PROXY_HPP_INCLUDED_20091206_
#define BOOST_CGI_DATA_MAP_PROXY_HPP_INCLUDED_20091206_

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {
 
  /// A proxy class to provide access to the data maps as member variables.
  /**
   * This wraps the underlying data map and exposes a std::map-like 
   * interface for the different data maps.
   *
   * It also includes an as<> member function which casts the found data
   * into a type the user specifies.
   */
  template<typename MapType>
  class data_map_proxy;

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_DATA_MAP_PROXY_HPP_INCLUDED_20091206_

