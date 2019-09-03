cmake_minimum_required (VERSION 3.5)

set(PROJECT_NAME "udp_server_test")
set(SRC_LIST src/udp_server.cpp tests/udp_server_test.cpp)
#set(PROJECT_NAME "udp_client_test")
#set(SRC_LIST src/udp_client.cpp tests/udp_client_test.cpp)



set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wshadow -Wundef -Wno-missing-field-initializers")

project (${PROJECT_NAME})

include_directories(include)

add_executable(${PROJECT_NAME} ${SRC_LIST})
