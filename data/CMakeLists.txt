
project(ValueLib_data)

find_package(Threads)

set(SRCS)
set(BINARY_SRCS)
add_subdirectory(include)
set(INTERFACE_FILES ${SRCS} ${BINARY_SRCS})
list(APPEND ALL_SRCS ${SRCS})
list(APPEND ALL_BINARIES ${BINARY_SRCS})

set(SRCS)
set(BINARY_SRCS)
add_subdirectory(src)
set(SOURCE_FILES ${SRCS} ${BINARY_SRCS})
list(APPEND ALL_SRCS ${SRCS})
list(APPEND ALL_BINARIES ${BINARY_SRCS})

set(SRCS)
set(BINARY_SRCS)
add_subdirectory(test)
set(TEST_FILES ${SRCS} ${BINARY_SRCS})
list(APPEND ALL_SRCS ${SRCS})
list(APPEND ALL_BINARIES ${BINARY_SRCS})

SOURCE_GROUP_BY_FOLDER(${ALL_SRCS})
BINARY_GROUP_BY_FOLDER(${ALL_BINARIES})

add_library(ValueLib_data ${INTERFACE_FILES} ${SOURCE_FILES})
target_link_libraries(ValueLib_data ValueLib_immutable)
target_include_directories(ValueLib_data
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
    EXPORT "ValueLib_data"
    DESTINATION "lib"
)

install(
    TARGETS ValueLib_data
    EXPORT "ValueLib_data"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "include")

find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
	add_executable(ValueLib_data_test ${TEST_FILES})
	target_link_libraries(ValueLib_data_test ${CMAKE_THREAD_LIBS_INIT} ValueLib_data ${GTEST_BOTH_LIBRARIES})
    add_test(NAME test_all_data COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ValueLib_data_test)
endif()


