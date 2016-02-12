macro (add_sources)
    foreach (_src ${ARGN})
        list (APPEND SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${_src}")
    endforeach()
    set (SRCS ${SRCS} PARENT_SCOPE)
endmacro()

macro (add_binaries)
    foreach (_src ${ARGN})
        list (APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/${_src}")
    endforeach()
    set (BINARY_SRCS ${BINARY_SRCS} PARENT_SCOPE)
endmacro()

MACRO(SOURCE_GROUP_BY_FOLDER)
    FOREACH(file ${ARGN})
        file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_SOURCE_DIR}" ${file})
        GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
        SOURCE_GROUP(${dir} FILES ${file})
    ENDFOREACH(file)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)

MACRO(BINARY_GROUP_BY_FOLDER)
    FOREACH(file ${ARGN})
        file(RELATIVE_PATH relative_file "${CMAKE_CURRENT_BINARY_DIR}" ${file})
        GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
        SOURCE_GROUP(${dir} FILES ${file})
    ENDFOREACH(file)
ENDMACRO(BINARY_GROUP_BY_FOLDER)
