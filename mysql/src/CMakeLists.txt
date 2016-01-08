add_subdirectory(connection)
add_subdirectory(transaction)
add_subdirectory(statement)

set(PRIVATE_SOURCE
   ${PRIVATE_SOURCE}
    ${CMAKE_CURRENT_LIST_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/connection.cpp


    ${CMAKE_CURRENT_SOURCE_DIR}/connection_invariants.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_buffer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/field_data.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/mysql_api.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/result_binder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/result_binder.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/error.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/execution_result.cpp


    ${CMAKE_CURRENT_SOURCE_DIR}/statement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transaction.cpp




   PARENT_SCOPE
)
set(PRIVATE_HEADERS
   ${PRIVATE_HEADERS}
   PARENT_SCOPE
)

