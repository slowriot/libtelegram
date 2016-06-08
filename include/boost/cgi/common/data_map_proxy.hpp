
#ifndef BOOST_CGI_DATA_MAP_PROXY_HPP_INCLUDED_20091206_
#define BOOST_CGI_DATA_MAP_PROXY_HPP_INCLUDED_20091206_

#include <exception>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
///////////////////////////////////////////////////////////
#include "boost/cgi/config.hpp"

/// Throws an exception if you try to use some request data without
/// loading the request first. This checking is only done in debug
/// builds and is a noop in release builds.
#ifndef BOOST_CGI_MAP_ASSERT
# ifndef NDEBUG
#   define BOOST_CGI_MAP_ASSERT(x) if (!x) throw map_read_error<map_type>()
# else
#   define BOOST_CGI_MAP_ASSERT(x)
# endif // NDEBUG
#endif // BOOST_CGI_MAP_ASSERT

BOOST_CGI_NAMESPACE_BEGIN

 namespace common {
 
  template<typename T>
  struct map_read_error
    : std::runtime_error
  {
    map_read_error()
      : std::runtime_error("Attempt to access uninitialised data map. Did you forget to call request::load()?")
    {
    }
  };

  /// A proxy class to provide access to the request data.
  /**
   * This class wraps the underlying data map and exposes a std::map-like 
   * interface for the different data maps.
   *
   * The class also adds some helper functions which are commonly used
   * for CGI programming, such as the member functions for looking up
   * items and casting them to different types.
   */
  template<typename MapType>
  class data_map_proxy
  {
  public:
    typedef MapType                                   map_type;
    typedef data_map_proxy<map_type>                  self_type;
    typedef typename map_type::key_type               key_type;
    typedef typename map_type::value_type             value_type;
    typedef typename map_type::mapped_type            mapped_type;
    typedef typename map_type::size_type              size_type;
    typedef typename map_type::iterator               iterator;
    typedef typename map_type::const_iterator         const_iterator;
    typedef typename map_type::reverse_iterator       reverse_iterator;
    typedef typename map_type::const_reverse_iterator const_reverse_iterator;
    typedef typename map_type::allocator_type         allocator_type;
    
    data_map_proxy()
      : impl_(NULL)
    {
    }

    /// Map iterators.
    
    iterator begin() { return impl().begin(); }
    iterator end() { return impl().end(); }
    const_iterator begin() const { return impl().begin(); }
    const_iterator end() const { return impl().end(); }
    reverse_iterator rbegin() { return impl().rbegin(); }
    reverse_iterator rend() { return impl().rend(); }
    const_reverse_iterator rbegin() const { return impl().rbegin(); }
    const_reverse_iterator rend() const { return impl().rend(); }

    iterator insert( iterator pos, const value_type& pair ) {
       return impl().insert(pos, pair);
    }
    
    template<typename InputIterator>
    void insert(InputIterator start, InputIterator end) {
       return impl().insert(start, end);
    }
    
    std::pair<iterator,bool> insert(const value_type& pair) {
       return impl().insert(pair);
    }
    
    void set(map_type& data) { impl_ = &data; }

    bool empty() { return impl().empty(); }

    void clear() { return impl().clear(); }

    size_type size() const { return impl().size(); }

    size_type count(const key_type& key) { return impl().count(key); }

    /// Fast check to see if a key exists
    bool exists(const key_type& key) { return impl().count(key) != 0; }

    /// Get a value for the key, with a fallback when not found.
    mapped_type const&
      pick(key_type const& key, mapped_type const& default_value) const
    {
      const_iterator iter = impl().find(key);
      return iter == impl().end() ? default_value : iter->second;
    }

    /// Get a value for the key as a specified type.
    /**
     * @param key   The name of CGI parameter to look for.
     *
     * If the key is found, attempts to convert the value into the type
     * T. This throws a boost::bad_lexical_cast when it fails.
     */
    template<typename T>
    T as(key_type const& key) const
    {
      return boost::lexical_cast<T>((impl())[key]);
    }

    /// Get a value for the key as a specified type, with fallback.
    /**
     * @param key   The name of CGI parameter to look for.
     * @param default_value
     *              The default return value. If no data exists in the map
     *              for the specified key, or the data cannot be converted
     *              into the type T, then this value is returned.
     *
     * If the key cannot be found, or the value cannot be lexically cast to
     * the type T, `pick` returns a default-constructed object of type T.
     * Otherwise it will return the value for that key as a type T.
     */
    template<typename T>
    T pick(key_type const& key, T const& default_value = T()) const
    {
      const_iterator iter = impl().find(key);

      T val (default_value);

      if (iter != impl().end()) {
        try {
          val = boost::lexical_cast<T>(iter->second);
        } catch(...) {
          // pass
        }
      }
      return val;
    }

    /// Look up `key` in the data map and return a reference to it.
    /**
     * This works just like a std::map<>, whereby if an item is not found
     * in the map then a new item is created with an empty value and a
     * reference to the item is returned.
     *
     * If you want to avoid creating an item, you can use `pick()` instead.
     */
    mapped_type& operator[](const char* key) {
      return impl()[key];
    }

    /// Look up `key` in the data map and return a reference to it.
    /**
     * This works just like a std::map<>, whereby if an item is not found
     * in the map then a new item is created with an empty value and a
     * reference to the item is returned.
     *
     * If you want to avoid creating an item, you can use `pick()` instead.
     */
    mapped_type& operator[](key_type const& key) {
      return impl()[key.c_str()];
    }

    /// Get access to the internal map structure.
    map_type& impl() { BOOST_CGI_MAP_ASSERT(impl_); return *impl_; }

    /// Get read-only access to the internal map structure.
    map_type const& impl () const { BOOST_CGI_MAP_ASSERT(impl_); return *impl_; }

    /// A data_map_proxy is implicitly convertible to the underlying map type.
    operator map_type&() { return impl(); }

    /// Check if the proxy contains a map.
    /**
     * This proxy class keeps a raw pointer to a request data map, which it
     * refers to. You set it using `set()` and can then use a boolean check
     * on the object to confirm that the internal pointer is assigned.
     */
    bool operator!() const { return !impl_; }

  protected:      
    map_type* impl_;
  };

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_DATA_MAP_PROXY_HPP_INCLUDED_20091206_

