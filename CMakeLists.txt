cmake_minimum_required(VERSION 3.10)
project(DBusConfigManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDBUS_C++ REQUIRED sdbus-c++)

pkg_check_modules(JSONCPP REQUIRED jsoncpp)

add_subdirectory(service)
add_subdirectory(confManagerApplication1)