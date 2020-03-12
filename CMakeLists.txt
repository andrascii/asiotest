cmake_minimum_required(VERSION 2.8.11)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)

set(AppName asiotest)
project(${AppName})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
set(CMAKE_INSTALL_PREFIX)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system date_time program_options filesystem COMPONENTS)
message("Boost found ${Boost_FOUND}")

add_subdirectory(${AppName})