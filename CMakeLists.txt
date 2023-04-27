cmake_minimum_required(VERSION 3.20)
project(osBase)

include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# source
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(OSBASE cho.osbase)
add_compile_definitions(NS_OSBASE=cho::osbase)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

