
#ifndef BOOST_CGI_UTILITY_SESSIONS_HPP_INCLUDED_
#define BOOST_CGI_UTILITY_SESSIONS_HPP_INCLUDED_

#include "boost/cgi/detail/push_options.hpp"

#include <string>
#include <fstream>
#include <iostream>
///////////////////////////////////////////////////////////
#include <boost/system/error_code.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>
#include <boost/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>
#ifndef BOOST_CGI_NO_BOOST_FILESYSTEM
#include <boost/filesystem.hpp>
#endif // BOOST_CGI_NO_BOOST_FILESYSTEM
///////////////////////////////////////////////////////////
#include "boost/cgi/import/write.hpp"
#include "boost/cgi/common/header.hpp"
#include "boost/cgi/common/protocol_traits.hpp"
#include "boost/cgi/detail/throw_error.hpp"
///////////////////////////////////////////////////////////

/// The directory to store sessions in.
/**
 * Your web server will need read and write permissions to this directory.
 * You are responsible for ensuring that files in this folder are protected!
 * 
 * Session support is currently experimental and relatively dumb.
 *
 * A file is saved for every user, containing the session data.
 * The session data itself defaults to a std::map<string,string>, which
 * is derived from the ProtocolTraits of the request.
 *
 * At the moment you are also responsible for maintaining this directory:
 * the library does nothing to clean up stale sessions. Session files are
 * only removed when the session is explicitly closed by a library user.
 *
 * In general you will need to write a script which deletes session files
 * that have not been modified in the last N days... Need clearer docs on
 * this.
 *
 * Look at the `sessions` example to see how to use your own types
 * for sessions. In general, the idea is that any type that is both
 * DefaultConstructable and also Serializable (as defined by
 * Boost.Serialization) can be used as the session type with no runtime
 * overhead.
 */
#ifndef BOOST_CGI_SESSIONS_DIRECTORY
#  define BOOST_CGI_SESSIONS_DIRECTORY "../sessions"
#endif // BOOST_CGI_SESSIONS_DIRECTORY

#ifndef BOOST_CGI_SESSION_COOKIE_NAME
#  define BOOST_CGI_SESSION_COOKIE_NAME "_ssid"
#endif // BOOST_CGI_SESSION_COOKIE_NAME

BOOST_CGI_NAMESPACE_BEGIN
 namespace common {

template<typename T>
class basic_session : public T, private boost::noncopyable
{
public:
  typedef T           value_type;
  typedef std::string string_type;

  basic_session(string_type const& id = "")
    : id_(id)
    , loaded_(false)
  {}
  
  basic_session(T& t, string_type const& id = "")
    : T(t)
    , id_(id)
    , loaded_(false)
  {}
  
  basic_session(T const& t, string_type const& id = "")
    : T(t)
    , id_(id)
    , loaded_(false)
  {}

  /// Get the id for the session.
  string_type const& id () const { return id_; }
  /// Set the id for the session.
  void id (string_type const& new_id) { id_ = new_id; }

  /// A basic_session is implicitly convertible to it's value_type.
  operator T& () { return static_cast<T&>(*this); }
  /// A basic_session is implicitly convertible to it's value_type.
  operator T const& () const { return static_cast<T const&>(*this); }

  /// Check if the session has been loaded.
  bool const& loaded() const { return loaded_; }
  /// Set whether the session is loaded.
  void loaded(bool status) { loaded_ = status; }

private:
  string_type id_;
  bool loaded_;
};

///////////////////////////////////////////////////////////

template<typename Protocol>
class basic_session_manager
{
public:
  typedef Protocol                             protocol_type;
  typedef protocol_traits<protocol_type>       traits;
  typedef typename traits::string_type         string_type;
  typedef typename traits::uuid_generator_type uuid_generator_type;
  
  basic_session_manager(
    string_type const& save_dir = BOOST_CGI_SESSIONS_DIRECTORY)
    : save_dir(save_dir)
  {}

  template<typename T>
  void save(basic_session<T>& sesh)
  {
    std::ofstream ofs(
        (save_dir + "/" + sesh.id() + ".arc").c_str());
    if (ofs) {
      boost::archive::text_oarchive archive(ofs);
      archive<< static_cast<typename basic_session<T>::value_type&>(sesh);
    }
  }

  template<typename T>
  void load(basic_session<T>& sesh)
  {
    std::ifstream ifs(
        (save_dir + "/" + sesh.id() + ".arc").c_str());
    if (ifs) {
      boost::archive::text_iarchive archive(ifs);
      archive>> static_cast<typename basic_session<T>::value_type&>(sesh);
      sesh.loaded(true);
    }
  }
  
  template<typename T>
  void stop(basic_session<T>& sesh)
  {
#ifndef BOOST_CGI_NO_BOOST_FILESYSTEM
    namespace fs = boost::filesystem;
    if (sesh.loaded()) {
      fs::path file (save_dir + sesh.id() + ".arc");
      if (fs::exists(file))
        fs::remove(file);
      sesh.id("");
    }
#endif // BOOST_CGI_NO_BOOST_FILESYSTEM
  }
  
  template<typename T>
  void start(basic_session<T>& sesh, string_type const& ssid = "")
  {
    if (!sesh.loaded() && sesh.id().empty())
    {
      if (!ssid.empty()) {
        sesh.id(ssid);
        load(sesh);
      } else {
        sesh.id(make_session_id());
        sesh.loaded(true);
      }
    }  
  }
  
  /// Get a new UUID as a string, suitable as a session id.
  string_type make_session_id()
  {
    try {
        return boost::lexical_cast<string_type>(make_uuid());
    } catch (...) {
        // pass
    }
    return "";
  }

  /// Generate a new UUID.
  boost::uuids::uuid make_uuid() { return generator_(); }

private:
  string_type save_dir;
  uuid_generator_type generator_;
};

 } // namespace common
BOOST_CGI_NAMESPACE_END

#endif // BOOST_CGI_UTILITY_SESSIONS_HPP_INCLUDED_
