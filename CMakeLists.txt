cmake_minimum_required (VERSION 3.8)

project (asio9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/srcs/include)

add_executable(tcp_server "srcs/test/tcpserver.cpp")
add_executable(http_server "srcs/test/httpserver.cpp")
add_executable(tcp_client "srcs/test/tcpclient.cpp")
add_executable(websocket_server "srcs/test/websocketserver.cpp")

if(NOT Boost_FOUND)
    message("Not found Boost")
endif()

target_link_libraries(tcp_server PRIVATE ${Boost_LIBRARIES})
target_link_libraries(http_server PRIVATE ${Boost_LIBRARIES})
target_link_libraries(tcp_client PRIVATE ${Boost_LIBRARIES})

