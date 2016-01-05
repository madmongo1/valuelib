cmake_minimum_required(VERSION 3.4.0)
project(ValueLib)

enable_testing()

include(cmake/FindMySQL.cmake)
find_package(Boost 1.59)
find_package(GTest)


add_subdirectory(pointers)
add_subdirectory(stdext)
add_subdirectory(database)
if(MYSQL_FOUND)
	add_subdirectory(mysql)
endif()


