
project(ValueLib_tuple)

find_package(Threads)

interface_tree(include)
source_tree(src)
test_tree(test)

add_library(ValueLib_tuple ${INTERFACE_FILES} ${SOURCE_FILES})
target_link_libraries(ValueLib_tuple ValueLib_immutable ValueLib_debug)

target_include_directories(ValueLib_tuple
SYSTEM PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/src
)

install(
    EXPORT "ValueLib_tuple"
    DESTINATION "lib"
)

install(
    TARGETS ValueLib_tuple
    EXPORT "ValueLib_tuple"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "include")

find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
	add_executable(ValueLib_tuple_test ${TEST_FILES})
	target_link_libraries(ValueLib_tuple_test ${CMAKE_THREAD_LIBS_INIT} ValueLib_tuple ${GTEST_BOTH_LIBRARIES})
    add_test(NAME test_all_tuple COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ValueLib_tuple_test)
endif()


