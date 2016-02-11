
project(ValueLib_immutable)

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

add_library(ValueLib_immutable ${INTERFACE_FILES} ${SOURCE_FILES})
target_include_directories(ValueLib_immutable 
PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	$<INSTALL_INTERFACE:include>
PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/include
	${CMAKE_CURRENT_BINARY_DIR}/src
)

install(
    TARGETS ValueLib_immutable
    EXPORT "ValueLib_immutable"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "include")

export(EXPORT ValueLib_immutable)


find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
	add_executable(ValueLib_immutable_test ${TEST_FILES})
	target_link_libraries(ValueLib_immutable_test ValueLib_immutable ${GTEST_BOTH_LIBRARIES})
    add_test(NAME test_all_immutable COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ValueLib_immutable_test)
endif()


