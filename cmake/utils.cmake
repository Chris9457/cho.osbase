function(get_dependencies)
	set(options "")
	set(oneValueArgs DEPENDENCIES)
	set(multiValueArgs "")
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg DEPENDENCIES)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "set_dependencies: missing params => ${missing_params}")
	endif()
	##########################################

	file(GLOB FILE_DEPENDENCIES ${CMAKE_CURRENT_BINARY_DIR}/*.dll)
	set(${ARG_DEPENDENCIES} ${FILE_DEPENDENCIES} PARENT_SCOPE)
endfunction()