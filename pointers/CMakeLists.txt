project(ValueLib_pointers)

interface_tree(include)
source_tree(src)
test_tree(test)

add_library(ValueLib_pointers ${INTERFACE_FILES} ${SOURCE_FILES})
target_include_directories(ValueLib_pointers PUBLIC
SYSTEM PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<INSTALL_INTERFACE:include>
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/src
)

find_package(GTest)
find_package(Threads)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
    add_executable(ValueLib_pointers_test ${TEST_SOURCE} ${TEST_HEADERS})
    add_test(NAME test_all_pointers COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ValueLib_pointers_test)
    target_link_libraries(ValueLib_pointers_test ${CMAKE_THREAD_LIBS_INIT} ValueLib_pointers ${GTEST_BOTH_LIBRARIES})
endif()
