cmake_minimum_required(VERSION 3.4.0)
project(ValueLib)

enable_testing()

find_package(Boost 1.59)
find_package(GTest)


add_subdirectory(pointers)
add_subdirectory(stdext)


