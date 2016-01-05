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

set(SUBORDINATE_LIBRARIES )
if (MYSQL_FOUND)
    list(APPEND SUBORDINATE_LIBRARIES ${MYSQL_LIBRARIES})
    list(APPEND SUBORDINATE_SYSTEM_INCLUDES ${MYSQL_INCLUDE_DIR})
else()
    message(FATAL "mysql not found")
endif()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)


add_library(database ${PUBLIC_HEADERS} ${CONFIGURED_PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE})
target_include_directories(database 
PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
$<INSTALL_INTERFACE:include/valuelib/database>
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/database
${CMAKE_CURRENT_BINARY_DIR}/database
PRIVATE SYSTEM
${SUBORDINATE_SYSTEM_INCLUDES}
)
target_link_libraries(database ${SUBORDINATE_LIBRARIES})


source_group_by_folder(${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE} ${TEST_SOURCE} ${TEST_HEADERS})
binary_group_by_folder(${CONFIGURED_PUBLIC_HEADERS})


find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
    add_executable(database_test ${TEST_SOURCE} ${TEST_HEADERS})
    add_test(NAME test_all_database COMMAND ${CMAKE_CURRENT_BINARY_DIR}/database_test)
    target_link_libraries(database_test database ${GTEST_BOTH_LIBRARIES})
endif()
