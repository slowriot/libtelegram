cmake_minimum_required (VERSION 3.12)

set(PROJECT_NAME "libtelegram-example")
project (${PROJECT_NAME})

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

find_package(Boost 1.66 REQUIRED COMPONENTS system)
IF(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
ELSE()
    message(FATAL "${CMAKE_SOURCE_DIR}/../include")
ENDIF(Boost_FOUND)

find_package(OpenSSL REQUIRED)

set(include_dir "${CMAKE_SOURCE_DIR}/../include")
message(STATUS "include_dir: ${include_dir}")
include_directories (${include_dir})

set(link_libs 
    Threads::Threads
    OpenSSL::SSL
    Boost::system)

macro(add_tg_example name)
    set(target_name ${name})
    add_executable(${target_name} ${name}.cpp)
    target_link_libraries(${target_name} ${link_libs})
endmacro(add_tg_example name)

add_tg_example(calculator)
add_tg_example(echo)
add_tg_example(echo_json)
add_tg_example(getphoto)
add_tg_example(livelocation)
add_tg_example(upload)
add_tg_example(webfetch)
