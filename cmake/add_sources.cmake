macro (add_sources)
    foreach (_src ${ARGN})
        list (APPEND SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
    endforeach()
    set (SRCS ${SRCS} PARENT_SCOPE)
endmacro()

macro (add_binaries)
    foreach (_src ${ARGN})
        list (APPEND BINARY_SRCS "${CMAKE_CURRENT_BINARY_DIR}/${_src}")
    endforeach()
    set (BINARY_SRCS ${BINARY_SRCS} PARENT_SCOPE)
endmacro()

macro (add_configured_source infile outfile)
    configure_file(${infile} ${outfile})
    add_binaries(${outfile})
    list (APPEND NOCOMPILE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${infile}")
    set (NOCOMPILE_SOURCES ${NOCOMPILE_SOURCES} PARENT_SCOPE)
endmacro()


MACRO(SOURCE_GROUP_BY_FOLDER)
    FOREACH(file ${ARGN})
        file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_SOURCE_DIR}" ${file})
        GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
        STRING(REPLACE "/" "\\\\" group_name ${dir})
        SOURCE_GROUP(${group_name} FILES ${file})
    ENDFOREACH(file)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)

MACRO(BINARY_GROUP_BY_FOLDER)
    FOREACH(file ${ARGN})
        file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_BINARY_DIR}" ${file})
        GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
        STRING(REPLACE "/" "\\\\" group_name ${dir})
        SOURCE_GROUP(${group_name} FILES ${file})
    ENDFOREACH(file)
ENDMACRO(BINARY_GROUP_BY_FOLDER)

MACRO(interface_tree subdir)
    set(SRCS)
    set(BINARY_SRCS)
    set(NOCOMPILE_SOURCES)
    add_subdirectory(${subdir})
    list(APPEND INTERFACE_FILES ${SRCS} ${BINARY_SRCS})
    list(APPEND SOURCE_FILES ${NOCOMPILE_SOURCES})
    source_group_by_folder(${SRCS} ${NOCOMPILE_SOURCES})
    binary_group_by_folder(${BINARY_SRCS})
    set_source_files_properties(${SRCS} ${BINARY_SRCS} ${NOCOMPILE_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)
    list(APPEND 
        DOCUMENTATION_INCLUDE_PATHS 
        "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}" 
        "${CMAKE_CURRENT_BINARY_DIR}/${subdir}"
        )
    set(DOCUMENTATION_INCLUDE_PATHS ${DOCUMENTATION_INCLUDE_PATHS} PARENT_SCOPE)

    list(APPEND
        DOCUMENTATION_SOURCE_FILES
        ${SRCS} ${BINARY_SRCS}
        )
    set(DOCUMENTATION_SOURCE_FILES ${DOCUMENTATION_SOURCE_FILES} PARENT_SCOPE)

    list(APPEND DOCUMENTATION_STRIP_FROM_PATHS
        "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}" 
        "${CMAKE_CURRENT_BINARY_DIR}/${subdir}"
    )
    set(DOCUMENTATION_STRIP_FROM_PATHS ${DOCUMENTATION_STRIP_FROM_PATHS} PARENT_SCOPE)
    
ENDMACRO()

MACRO(source_tree subdir)
    set(SRCS)
    set(BINARY_SRCS)
    set(NOCOMPILE_SOURCES)
    add_subdirectory(${subdir})
    list(APPEND SOURCE_FILES ${SRCS} ${BINARY_SRCS})
    list(APPEND SOURCE_FILES ${NOCOMPILE_SOURCES})
    source_group_by_folder(${SRCS} ${NOCOMPILE_SOURCES})
    binary_group_by_folder(${BINARY_SRCS})
    set_source_files_properties(${NOCOMPILE_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)
    list(APPEND 
        DOCUMENTATION_INCLUDE_PATHS 
        "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}" 
        "${CMAKE_CURRENT_BINARY_DIR}/${subdir}"
        )
    set(DOCUMENTATION_INCLUDE_PATHS ${DOCUMENTATION_INCLUDE_PATHS} PARENT_SCOPE)

    list(APPEND
        DOCUMENTATION_SOURCE_FILES
        ${SRCS} ${BINARY_SRCS}
        )
    set(DOCUMENTATION_SOURCE_FILES ${DOCUMENTATION_SOURCE_FILES} PARENT_SCOPE)

    list(APPEND DOCUMENTATION_STRIP_FROM_PATHS
        "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}" 
        "${CMAKE_CURRENT_BINARY_DIR}/${subdir}"
    )
    set(DOCUMENTATION_STRIP_FROM_PATHS ${DOCUMENTATION_STRIP_FROM_PATHS} PARENT_SCOPE)
ENDMACRO()

MACRO(test_tree subdir)
    set(SRCS)
    set(BINARY_SRCS)
    set(NOCOMPILE_SOURCES)
    add_subdirectory(${subdir})
    list(APPEND TEST_FILES ${SRCS} ${BINARY_SRCS})

    list(APPEND TEST_FILES ${NOCOMPILE_SOURCES})
    source_group_by_folder(${SRCS} ${NOCOMPILE_SOURCES})
    binary_group_by_folder(${BINARY_SRCS})
    set_source_files_properties(${NOCOMPILE_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)
ENDMACRO()

#intermediatary outputs ARE:
# SRCS = source files to be compiled from the source tree
# BINARY_SRCS = source files to be compiled from the binary tree
# NOCOMPILE_SOURCES = sourcefiles in the source tree that should not be compiled


# FINAL outputs are
# INTERFACE_FILES = files that are part of the (library) interface
# SOURCE_FILES = files that build the library/executable
# TEST_FILES = files that build the test
# DOCUMENTATION_INCLUDE_PATHS - set in the parent scope
# DOCUMENTATION_SOURCE_FILES - set in the parent scope
# DOCUMENTATION_STRIP_FROM_PATHS - set in parent scope
