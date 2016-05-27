
project(ValueLib_immutable)

find_package(Threads)

interface_tree(include)
source_tree(src)
test_tree(test)

add_library(ValueLib_immutable ${INTERFACE_FILES} ${SOURCE_FILES})
target_link_libraries(ValueLib_immutable ValueLib_debug)
target_include_directories(ValueLib_immutable 
SYSTEM PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<INSTALL_INTERFACE:include>
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/src
)

install(
    EXPORT "ValueLib_immutable"
    DESTINATION "lib"
)

install(
    TARGETS ValueLib_immutable
    EXPORT "ValueLib_immutable"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "include")

find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
	add_executable(ValueLib_immutable_test ${TEST_FILES})
	target_link_libraries(ValueLib_immutable_test ValueLib_immutable 
        ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    add_test(NAME test_all_immutable COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ValueLib_immutable_test)
endif()


