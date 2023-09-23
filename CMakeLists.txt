cmake_minimum_required(VERSION 3.10.0)
project(HikkaRR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_compile_definitions(__cpp_lib_format=201907L)
    set(INCLUDE_DIR "D:/soft/VisualStudio/VC/Tools/MSVC/14.29.30133/include")

    set(dpp_DIR ${INCLUDE_DIR}/dpp/lib/cmake/dpp)
    set(dpp_LIB ${INCLUDE_DIR}/dpp/lib/dpp.lib)
    set(Boost_INCLUDE_DIR "D:/soft/VisualStudio/VC/Tools/MSVC/14.29.30133/include")
    set(Boost_LIBRARIES_CMAKE ${Boost_INCLUDE_DIR}/stage/lib)
    set(OPENSSL_ROOT_DIR "D:/soft/VisualStudio/VC/Tools/MSVC/14.29.30133/include/openssl/")
    set(OPENSSL_INCLUDE_DIR "D:/soft/VisualStudio/VC/Tools/MSVC/14.29.30133/include/openssl/include/")
    set(OPENSSL_LIBRARIES "D:/soft/VisualStudio/VC/Tools/MSVC/14.29.30133/include/openssl/lib/")
    set(OPENSSL_CRYPTO_LIBRARY "D:/soft/VisualStudio/VC/Tools/MSVC/14.29.30133/include/openssl/lib/")

    link_directories(${OPENSSL_LIBRARIES_CMAKE})
    include_directories(${INCLUDE_DIR})
else()
    link_directories(depends/lib/)
    set(dpp_LIB libdpp.so libsodium.so libopus.so)
    set(MYSQL_LIBS libmysqlcppconn.so libmysqlcppconn8.so)
    set(SHIT_CURLPP_LIBS libcurlpp.so libcurlpp.so.1 libcurlpp.so.1.0.0 curl)
    set(OGGZ_LIBS liboggz.so)
endif()

find_package(OpenSSL 3.0.0 REQUIRED)
find_package(Boost 1.80.0)
#find_package(liboggz REQUIRED) я не нашёл

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_CRYPTO_LIBRARY_USE_STATIC_LIBS TRUE)
set(dpp_USE_STATIC_RUNTIME ON)
set(dpp_USE_STATIC_LIBS ON)

FILE(GLOB COMMANDS_FILES commands/*.cpp)

add_executable(HikkaRR
        client/Threading/ThreadManager.cpp

    client/player/player.cpp
    client/player/Sources/SharedFile.cpp

    events/on_ready.hpp events/on_ready.cpp
    events/on_slashcommand.hpp events/on_slashcommand.cpp

    util/SysAPI.h util/SysAPI.cpp
    util/database.cpp

    client/player/Sources/YTVideo.cpp
    client/player/Sources/Source.cpp

    ${COMMANDS_FILES}

    main.cpp
)

include_directories(./ ./depends/include)

target_link_libraries(HikkaRR   ${Boost_LIBRARIES}
                                ${OPENSSL_LIBRARIES}
                                ${dpp_LIB}
                                ${MYSQL_LIBS}
                                ${SHIT_CURLPP_LIBS}
                                ${OGGZ_LIBS}
)
