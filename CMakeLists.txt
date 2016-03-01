cmake_minimum_required(VERSION 3.4.0)
project(ValueLib)

set(ValueLib_Folder_Prefix)
if (NOT CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  set(ValueLib_Folder_Prefix "ValueLib/")
endif()

enable_testing()

if (NOT Boost_INCLUDE_DIR)
	find_package(Boost 1.59)
endif()

find_package(GTest)

include(cmake/add_sources.cmake)
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

	add_custom_target(doc ALL
					  ${DOXYGEN_EXECUTABLE} 
					  ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
					  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
					  COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()
