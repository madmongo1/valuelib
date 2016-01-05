MACRO(SOURCE_GROUP_BY_FOLDER)
    FOREACH(file ${ARGN})
        file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_SOURCE_DIR}" ${file})
        GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
        string(REPLACE "/" "\\\\" folder ${dir})
        SOURCE_GROUP(${folder} FILES ${file})
    ENDFOREACH(file)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)

MACRO(BINARY_GROUP_BY_FOLDER)
    FOREACH(file ${ARGN})
        file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_BINARY_DIR}" ${file})
        GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
        string(REPLACE "/" "\\\\" folder ${dir})
        SOURCE_GROUP(${folder} FILES ${file})
    ENDFOREACH(file)
ENDMACRO(BINARY_GROUP_BY_FOLDER)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)


add_library(mysql ${PUBLIC_HEADERS} ${CONFIGURED_PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE})
target_include_directories(mysql 
PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
$<INSTALL_INTERFACE:include/valuelib/database>
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_BINARY_DIR}/include
PRIVATE SYSTEM
${MYSQL_INCLUDE_DIR}
)
target_link_libraries(mysql ${MYSQL_LIBRARIES})


source_group_by_folder(${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE} ${TEST_SOURCE} ${TEST_HEADERS})
binary_group_by_folder(${CONFIGURED_PUBLIC_HEADERS})


find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
    add_executable(mysql_test ${TEST_SOURCE} ${TEST_HEADERS})
    add_test(NAME test_all_mysql COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mysql_test)
    target_link_libraries(mysql_test mysql ${GTEST_BOTH_LIBRARIES})
endif()
