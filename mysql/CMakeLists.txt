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

set(Boost_USE_STATIC_LIBS YES)
find_package(Boost 1.60 COMPONENTS thread system)

add_library(mysql ${PUBLIC_HEADERS} ${CONFIGURED_PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE})
target_include_directories(mysql 
PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
$<INSTALL_INTERFACE:include/valuelib/database>
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_BINARY_DIR}/src
)
target_include_directories(mysql
SYSTEM BEFORE PRIVATE
${Boost_INCLUDE_DIR}
${MYSQL_INCLUDE_DIR}
)
target_link_libraries(mysql ${MYSQL_LIBRARIES} ${Boost_LIBRARIES})
message(SYSTEM " libs: " ${Boost_LIBRARIES})
message(SYSTEM " inc: " ${Boost_INCLUDE_DIR})

source_group_by_folder(${PUBLIC_HEADERS} ${PRIVATE_HEADERS} ${PRIVATE_SOURCE} ${TEST_SOURCE} ${TEST_HEADERS})
binary_group_by_folder(${CONFIGURED_PUBLIC_HEADERS})


find_package(GTest)

if(GTEST_FOUND)
    add_executable(mysql_test ${TEST_SOURCE} ${TEST_HEADERS})
    target_include_directories(mysql_test
        SYSTEM BEFORE PRIVATE
        ${Boost_INCLUDE_DIR}
        ${GTEST_INCLUDE_DIRS}
        )
    add_test(NAME test_all_mysql COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mysql_test)
    target_link_libraries(mysql_test mysql ${GTEST_BOTH_LIBRARIES})
endif()
