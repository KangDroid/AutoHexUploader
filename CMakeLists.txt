cmake_minimum_required(VERSION 3.17)
project(AutoHexProject)

if (APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    set(OPENSSL_LIBRARIES "/usr/local/opt/openssl/lib")
endif()

find_package(cpprestsdk REQUIRED)
find_package (OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

set(CMAKE_CXX_FLAGS "-Wall -g -O3 -std=gnu++2a -Wno-deprecated-declarations -Wno-unneeded-internal-declaration")

include_directories(${AutoHexProject_SOURCE_DIR}/include)
#include_directories(/usr/local/Cellar/openssl\@1.1/1.1.1g/include)
include_directories(/usr/local/include)
ADD_EXECUTABLE( app.out src/main.cpp src/ArgumentParser.cpp src/BasicvariableInfo.cpp src/GithubRequestManager.cpp src/Logger.cpp src/PrinterInfo.cpp src/Timer.cpp src/WebRequestManager.cpp )
TARGET_LINK_LIBRARIES( app.out PRIVATE ${AutoHexProject_SOURCE_DIR}/lib/jsoncpp.precompiled cpprestsdk::cpprest OpenSSL::SSL Boost::thread Boost::system Boost::chrono OpenSSL::Crypto)