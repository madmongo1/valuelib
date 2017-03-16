set(sub_library "ValueLib_debug")
set(dependencies ValueLib_immutable ValueLib_debug)
project("${sub_library}")

find_package(Threads)
find_package(Boost REQUIRED)

set(PUBLIC_INTERFACE)
set(PRIVATE_INTERFACE)
prefix_files(VALUELIB_DEBUG PREFIX "include/valuelib/debug" APPEND PRIVATE_INTERFACE FILES
		any_printer.hpp
        chrono.hpp demangle.hpp emit_args.hpp exception.hpp method.hpp object.hpp print.hpp trace.hpp
        typeinfo.hpp unwrap.hpp vector.hpp)

prefix_files(SRC PREFIX "src" APPEND SOURCE_FILES FILES
        any_printer.cpp demangle.cpp trace.cpp unwrap.cpp)

prefix_files(TEST PREFIX "test" APPEND TEST_FILES FILES
        testDemangle.cpp)


add_library(${sub_library} ${PUBLIC_INTERFACE} ${PRIVATE_INTERFACE} ${SOURCE_FILES})
target_link_libraries(${sub_library} INTERFACE ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${sub_library} SYSTEM INTERFACE ${Boost_DIRECTORIES})

target_include_directories(${sub_library}
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
    EXPORT "${sub_library}"
    DESTINATION "lib"
)

install(
    TARGETS ${sub_library}
    EXPORT "${sub_library}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "include")

find_package(GTest)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
	add_executable("${sub_library}_test" ${TEST_FILES})
	target_link_libraries(ValueLib_debug_test ValueLib_debug 
            ${GTEST_BOTH_LIBRARIES})
    add_test(NAME "test_${sub_library}" COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${sub_library}_test")
endif()


