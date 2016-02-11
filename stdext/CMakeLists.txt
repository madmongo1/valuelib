project(ValueLib_stdext)

MACRO(SOURCE_GROUP_BY_FOLDER)
    FOREACH(file ${ARGN})
        file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_SOURCE_DIR}" ${file})
        GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
        SOURCE_GROUP("${dir}" FILES ${file})
    ENDFOREACH(file)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)



add_subdirectory(stdext)
add_subdirectory(src)
add_subdirectory(test)

source_group_by_folder(${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE} ${TEST_SOURCE} ${TEST_HEADERS})


add_library(ValueLib_stdext ${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE})
target_include_directories(ValueLib_stdext PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
$<INSTALL_INTERFACE:include/valuelib/stdext>
)

find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
    add_executable(ValueLib_stdext_test ${TEST_SOURCE} ${TEST_HEADERS})
    add_test(NAME test_all_stdext COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ValueLib_stdext_test)
    target_link_libraries(ValueLib_stdext_test ValueLib_stdext ${GTEST_BOTH_LIBRARIES})
endif()
