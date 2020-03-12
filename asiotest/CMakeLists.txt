cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE STRING INTERNAL FORCE)
set(CMAKE_CXX_STANDARD 20)

project(${AppName})

set(AppSourcesDir src)
set(AppHeadersDir include)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppHeadersList "include/*.h")

if(MSVC)
    add_definitions(
        /await
        -DBOOST_ASIO_DISABLE_CONCEPTS
    )
endif()

add_executable(
    ${AppName}
    ${AppSourcesList}
    ${AppHeadersList}
)

include_directories(
    ${AppHeadersDir}
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
    ${AppName}
    ${Boost_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_ASIO_LIBRARY}
)