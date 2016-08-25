# Make sure we're using correct version of CMake
cmake_minimum_required(VERSION 3.5)

# Check if we're a subproject with a parent and add prequisites if not
get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    include (CheckCXXCompilerFlag)
    enable_testing()
    include(sanity/cmake/sanity.cmake)
endif()

project(ValueLib)

#
# c++ version
#

if (NOT CMAKE_CXX_STANDARD)
    set (CMAKE_REQUIRED_QUIET FALSE)
    
    CHECK_CXX_COMPILER_FLAG(-std=c++17 supports17)
    CHECK_CXX_COMPILER_FLAG(-std=c++14 supports14)
    CHECK_CXX_COMPILER_FLAG(-std=c++11 supports11)
    if (supports17)
        set (CMAKE_CXX_STANDARD "14")
    elseif (supports14)
        set (CMAKE_CXX_STANDARD "14")
    elseif (supports11)
        set (CMAKE_CXX_STANDARD "11")
    else ()
        message (FATAL_ERROR "here's a nickel kid, go buy yourself a better compiler")
    endif ()
    set_property (GLOBAL PROPERTY CXX_STANDARD ${CMAKE_CXX_STANDARD})
    set (CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD} CACHE STRING "best complete standard supported by the compiler")
endif ()



#
# Libraries
#

sanity_require(LIBRARY boost VERSION any)
sanity_require(LIBRARY gtest VERSION any)

set(ValueLib_Folder_Prefix)
if (NOT CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  set(ValueLib_Folder_Prefix "ValueLib/")
endif()

enable_testing()

add_subdirectory(debug)
add_subdirectory(immutable)
add_subdirectory(tuple)
add_subdirectory(data)
add_subdirectory(pointers)
add_subdirectory(stdext)

find_package(Doxygen)
if(DOXYGEN_FOUND)

	#locate all the include paths for all file
	set(DOXYGEN_INCLUDE_PATHS "")
	foreach(_inc ${DOCUMENTATION_INCLUDE_PATHS})
		STRING(CONCAT DOXYGEN_INCLUDE_PATHS "${DOXYGEN_INCLUDE_PATHS}" "INCLUDE_PATH+=\"${_inc}\"\n")
	endforeach()

	set(DOXYGEN_INPUT_FILES "")
	foreach(_src ${DOCUMENTATION_SOURCE_FILES})
		STRING(CONCAT DOXYGEN_INPUT_FILES "${DOXYGEN_INPUT_FILES}" "INPUT+=\"${_src}\"\n")
	endforeach()

	set(DOXYGEN_STRIP_FROM_PATH "")
	foreach(_pth ${DOCUMENTATION_STRIP_FROM_PATHS})
		STRING(CONCAT DOXYGEN_STRIP_FROM_PATH "${DOXYGEN_STRIP_FROM_PATH}" "STRIP_FROM_PATH+=\"${_pth}\"\n")
	endforeach()

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in 
				   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

	add_custom_target(ValueLib_doc
					  ${DOXYGEN_EXECUTABLE} 
					  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
					  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					  COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()
