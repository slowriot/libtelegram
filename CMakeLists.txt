cmake_minimum_required(VERSION 3.10)

project(client)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost 1.66 REQUIRED)
IF(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
ELSE()
  message(FATAL "${CMAKE_SOURCE_DIR}/../include")
ENDIF(Boost_FOUND)

include_directories(SYSTEM BEFORE include)
include_directories(BEFORE .)

set(link_libs
  PRIVATE Threads::Threads
  PRIVATE OpenSSL::Crypto
  PRIVATE OpenSSL::SSL
)

IF(WIN32)
  # sigaction is not available on Windows - see "signal handlers" in README
  add_compile_definitions(TELEGRAM_NO_SIGNAL_HANDLER)
ENDIF()

macro(add_tg_example name)
  add_executable(example_${name} examples/${name}.cpp)
  target_link_libraries(example_${name} ${link_libs})
endmacro(add_tg_example name)

add_tg_example(echo)
add_tg_example(echo_json)
add_tg_example(webfetch)
add_tg_example(getphoto)
add_tg_example(calculator)
add_tg_example(upload)
add_tg_example(livelocation)
