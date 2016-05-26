project(ValueLib_stdext)

interface_tree(include)
source_tree(src)
test_tree(test)


add_library(ValueLib_stdext ${INTERFACE_FILES} ${SOURCE_FILES})
target_link_libraries(ValueLib_stdext ValueLib_tuple ValueLib_debug)

target_include_directories(
    ValueLib_stdext
    SYSTEM PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

install(
    EXPORT "ValueLib_stdext"
    DESTINATION "lib"
    )

install(
    TARGETS ValueLib_stdext
    EXPORT "ValueLib_stdext"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "include")


find_package(GTest)
find_package(Threads)

if(GTEST_FOUND)
    include_directories(BEFORE SYSTEM ${GTEST_INCLUDE_DIRS})
    add_executable(ValueLib_stdext_test ${TEST_FILES})
    target_link_libraries(ValueLib_stdext_test
        ${CMAKE_THREAD_LIBS_INIT}
        ValueLib_stdext
        ${GTEST_BOTH_LIBRARIES})
    add_test(NAME
        test_all_stdext
        COMMAND ValueLib_stdext_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
