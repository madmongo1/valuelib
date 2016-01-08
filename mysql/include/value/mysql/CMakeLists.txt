set(PUBLIC_HEADERS
	${PUBLIC_HEADERS}
   	${CMAKE_CURRENT_LIST_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/connection.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/connection_invariants.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/error.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/execution_result.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extract_optional.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/mysql_api.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/optional.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/result_set.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/statement.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/transaction.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/types.hpp
   	PARENT_SCOPE
)


