cmake_minimum_required (VERSION 3.5)

set(CLIENT_NAME "udp_client_test")
set(SERVER_NAME "udp_server_test")

set(CLIENT_SRC_LIST src/udp_client.cpp tests/udp_client_test.cpp)
set(SERVER_SRC_LIST src/udp_server.cpp tests/udp_server_test.cpp)

set(USR_LOCAL_PATH "/usr/local")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ggdb -Wextra -Wpedantic -Wshadow -Wundef -Wno-missing-field-initializers")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${USR_LOCAL_PATH}/include")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${USR_LOCAL_PATH}/lib -lwolfssl")


include_directories(include)

add_executable(${SERVER_NAME} ${SERVER_SRC_LIST})
add_executable(${CLIENT_NAME} ${CLIENT_SRC_LIST})



