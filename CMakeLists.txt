cmake_minimum_required(VERSION 3.7)
project(webm_bot)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/discordpp)
add_subdirectory(lib/rest-beast)
add_subdirectory(lib/websocket-simpleweb)
add_subdirectory(lib/plugin-ratelimit)

set(SOURCE_FILES main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Boost 1.71.0 REQUIRED system date_time)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fno-omit-frame-pointer")

INCLUDE_DIRECTORIES(
        ${Boost_INCLUDE_DIRS}
        ${discordpp_SOURCE_DIR}
        ${discordpp-rest-beast_SOURCE_DIR}
	${discordpp-websocketpp_SOURCE_DIR}
	${discordpp-websocket-simpleweb_SOURCE_DIR}
        ${discordpp-plugin-ratelimit_SOURCE_DIR}
        ${json_SOURCE_DIR}
        ${OPENSSL_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES(
        ${PROJECT_NAME}
        ${Boost_LIBRARIES}
	sqlite3
        discordpp
        discordpp-rest-beast
	discordpp-websocket-simpleweb
        Threads::Threads
        ${OPENSSL_LIBRARIES}
)

#if (EXISTS ${CMAKE_SOURCE_DIR}/token.dat)
    #configure_file(token.dat ${CMAKE_CURRENT_BINARY_DIR} COPYONLY
#elseif (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/token.dat)
    #file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/token.dat)
#endif ()

#include_directories(${PROJECT_SOURCE_DIR} ${discordpp_SOURCE_DIR} )
