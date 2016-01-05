
set(PRIVATE_SOURCE
   ${PRIVATE_SOURCE}
    ${CMAKE_CURRENT_LIST_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/connection.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_instance.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/connection_invariants.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/connection_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_pool.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/error.hpp

   PARENT_SCOPE
)
set(PRIVATE_HEADERS
   ${PRIVATE_HEADERS}
   PARENT_SCOPE
)

