cmake_minimum_required(VERSION 3.4.0)
project(ValueLib)

set(ValueLib_Folder_Prefix)
if (NOT CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  set(ValueLib_Folder_Prefix "ValueLib/")
endif()

enable_testing()

if (NOT Boost_INCLUDE_DIR)
	find_package(Boost VERSION 1.59)
endif()

find_package(GTest)

include(cmake/add_sources.cmake)
add_subdirectory(debug)
add_subdirectory(immutable)
add_subdirectory(tuple)
add_subdirectory(data)
add_subdirectory(pointers)
add_subdirectory(stdext)


