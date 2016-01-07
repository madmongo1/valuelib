add_subdirectory(transaction)

set(PRIVATE_SOURCE
   ${PRIVATE_SOURCE}
    ${CMAKE_CURRENT_LIST_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/connection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_impl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_impl.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance_impl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance_pool.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/connection_invariants.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/result_binder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/result_binder.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/error.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/execution_result.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/mysql_api.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/statement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/transaction.cpp




   PARENT_SCOPE
)
set(PRIVATE_HEADERS
   ${PRIVATE_HEADERS}
   PARENT_SCOPE
)

