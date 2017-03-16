
project(ValueLib_debug)

find_package(Threads)
find_package(Boost REQUIRED)

set(PUBLIC_INTERFACE)
set(PRIVATE_INTERFACE)
prefix_files(VALUELIB_DEBUG PREFIX "include/valuelib/debug" APPEND PRIVATE_INTERFACE FILES
        chrono.hpp demangle.hpp emit_args.hpp exception.hpp method.hpp object.hpp print.hpp trace.hpp
        typeinfo.hpp unwrap.hpp vector.hpp)

prefix_files(SRC PREFIX "src" APPEND SOURCE_FILES FILES
        any_printer.hpp demangle.cpp trace.cpp unwrap.cpp)

prefix_files(TEST PREFIX "test" APPEND SOURCE_FILES FILES
        testDemangle.cpp)


add_library(ValueLib_debug ${PUBLIC_INTERFACE} ${PRIVATE_INTERFACE} ${SOURCE_FILES})
target_link_libraries(ValueLib_debug INTERFACE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(ValueLib_debug SYSTEM INTERFACE ${Boost_DIRECTORIES})

target_include_directories(ValueLib_debug
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
    EXPORT "ValueLib_debug"
    DESTINATION "lib"
)

install(
    TARGETS ValueLib_debug
    EXPORT "ValueLib_debug"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "include")

find_package(GTest)

if(GTEST_FOUND)
    prefix_files(TEST PREFIX "test" APPEND TEST_FILES FILES testDemangle.cpp)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
	add_executable(ValueLib_debug_test ${TEST_FILES})
	target_link_libraries(ValueLib_debug_test ValueLib_debug 
            ${GTEST_BOTH_LIBRARIES})
    add_test(NAME test_all_debug COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ValueLib_debug_test)
endif()


