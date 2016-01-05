set(PUBLIC_HEADERS
	${PUBLIC_HEADERS}
   	${CMAKE_CURRENT_LIST_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/pointer_traits.hpp
   	PARENT_SCOPE
)


set(CONFIGURATION_FILES config.hpp.in)

foreach(_in_file ${CONFIGURATION_FILES})
  	string(REGEX REPLACE "(.*)\\.in$" "\\1" _hpp_file ${_in_file})
  	message(STATUS ${_in_file} " becomes " ${_hpp_file})
	CONFIGURE_FILE(${_in_file} ${_hpp_file})

	set(CONFIGURED_PUBLIC_HEADERS
		${CONFIGURED_PUBLIC_HEADERS}
    	${CMAKE_CURRENT_BINARY_DIR}/${_hpp_file}
    	PARENT_SCOPE
	)

	set(PRIVATE_HEADERS
   		${PRIVATE_HEADERS}
    	${CMAKE_CURRENT_SOURCE_DIR}/${_in_file}
   		PARENT_SCOPE
	)

endforeach()


