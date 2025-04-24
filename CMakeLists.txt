cmake_minimum_required(VERSION 3.10)
project(DBusConfigManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(sdbus-c++ REQUIRED)

add_subdirectory(service)
#add_subdirectory(client)