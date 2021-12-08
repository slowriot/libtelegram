cmake_minimum_required(VERSION 3.10)

project(client)

include_directories(include SYSTEM BEFORE)
include_directories(. BEFORE)
link_directories(${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BOOST_ROOT "/usr/src/boost_1_76_0")

find_package(Threads REQUIRED)
find_package(Boost 1.76.0 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${BOOST_ROOT})

add_executable(example_echo examples/echo.cpp)
add_executable(example_echo_json examples/echo_json.cpp)
add_executable(example_webfetch examples/webfetch.cpp)
add_executable(example_getphoto examples/getphoto.cpp)
add_executable(example_calculator examples/calculator.cpp)
add_executable(example_upload examples/upload.cpp)
add_executable(example_livelocation examples/livelocation.cpp)

set(LIBS
  PRIVATE Threads::Threads
  PRIVATE OpenSSL::Crypto
  PRIVATE OpenSSL::SSL
)

target_link_libraries(example_echo ${LIBS})
target_link_libraries(example_echo_json ${LIBS})
target_link_libraries(example_webfetch ${LIBS})
target_link_libraries(example_getphoto ${LIBS})
target_link_libraries(example_calculator ${LIBS})
target_link_libraries(example_upload ${LIBS})
target_link_libraries(example_livelocation ${LIBS})
