add_subdirectory(database)

set(PUBLIC_HEADERS
	${PUBLIC_HEADERS}
   	${CMAKE_CURRENT_LIST_FILE}
   	PARENT_SCOPE
)
