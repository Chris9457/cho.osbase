set(PY_SCRIPT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/script")
set(PY_STUB_SCRIPT ${PY_SCRIPT_DIR}/cppstub.py)
set(PY_IMPL_SCRIPT ${PY_SCRIPT_DIR}/cppimpl.py)

set_property(GLOBAL PROPERTY PY_STUB_SCRIPT_PROPERTY  ${PY_STUB_SCRIPT})
set_property(GLOBAL PROPERTY PY_IMPL_SCRIPT_PROPERTY  ${PY_IMPL_SCRIPT})

####################################################################
# Install a python package
#	PACKAGES[in]		list of package name
####################################################################
function(osbase_service_install_python_packages)
	find_package (Python3 3.9 COMPONENTS Interpreter)

	if (NOT Python3_FOUND)
		message(WARNING "Python not installed")
		return()
	endif()

	set(options "")
	set(oneValueArgs "")
	set(multiValueArgs PACKAGES)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	##########################################
	# check mandatory params
	foreach(arg PACKAGES)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
		
	if (missing_params)
		message(FATAL_ERROR "osbase_service_install: missing params => ${missing_params}")
	endif()
	##########################################

	foreach(_pkg ${ARG_PACKAGES})
		message("Install python package: " ${_pkg})
		set(PY_CMD -m pip install ${_pkg})
		execute_process(COMMAND ${Python3_EXECUTABLE} ${PY_CMD})
	endforeach()
endfunction()

####################################################################
# Generate the stub part of a service
#	TARGET[in]					Target to add generated sources
#	API[in]						Input yml
#   API_DEPENDS[in]				optional - api dependencies
####################################################################
function(osbase_service_generate_stub)
	find_package (Python3 3.9 COMPONENTS Interpreter REQUIRED)

	if (NOT Python3_FOUND)
		message(WARNING "Python not installed")
		return()
	endif()

	set(options "")
	set(oneValueArgs TARGET API)
	set(multiValueArgs API_DEPENDS)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	##########################################
	# check mandatory params
	foreach(arg TARGET API)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
		
	if (missing_params)
		message(FATAL_ERROR "osbase_service_generate_stub: missing params => ${missing_params}")
	endif()
	##########################################

	if (NOT EXISTS ${ARG_API})
		set(ARG_API ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_API})
	endif()

	if (NOT EXISTS ${ARG_API})
		message(FATAL_ERROR "${ARG_API} not found!")
	endif()

	get_filename_component(apiFileName ${ARG_API} NAME_WLE)
	set(AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/autogen)
	set(HEADER ${AUTOGEN_BUILD_DIR}/${apiFileName}.h)
	set(CORE ${AUTOGEN_BUILD_DIR}/${apiFileName}.cpp)

	foreach (_file ${HEADER} ${CORE})
		if (NOT EXISTS ${_file})
			set(_execute_script 1)
		endif()
	endforeach()


	get_property(PY_STUB_SCRIPT GLOBAL PROPERTY PY_STUB_SCRIPT_PROPERTY)
	set(PY_CMD ${PY_STUB_SCRIPT} --outputdir ${AUTOGEN_BUILD_DIR} ${ARG_API})
	add_custom_command(
	OUTPUT
		${HEADER}
		${CORE}
	DEPENDS
		${ARG_API}
		${ARG_API_DEPENDS}
	COMMAND ${Python3_EXECUTABLE} ${PY_CMD})

	if (_execute_script)
		execute_process(COMMAND ${Python3_EXECUTABLE} ${PY_CMD})
	endif()

	target_sources(${ARG_TARGET} PUBLIC ${HEADER} PRIVATE ${CORE})
	target_include_directories(${ARG_TARGET} PUBLIC ${AUTOGEN_BUILD_DIR})
endfunction()

####################################################################
# Generate the implementation skeleton part of a service
#	TARGET[in]				Target to add generated sources
#	API[in]					Input yml
####################################################################
function(osbase_service_generate_impl_skeleton)
	find_package (Python3 COMPONENTS Interpreter)

	if (NOT Python3_FOUND)
		message(WARNING "Python not installed")
		return()
	endif()

	set(options "")
	set(oneValueArgs TARGET API)
	set(multiValueArgs "")
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	##########################################
	# check mandatory params
	foreach(arg TARGET API)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
		
	if (missing_params)
		message(FATAL_ERROR "osbase_service_generate_impl_skeleton: missing params => ${missing_params}")
	endif()
	##########################################


	if (NOT EXISTS ${ARG_API})
		set(ARG_API ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_API})
	endif()

	if (NOT EXISTS ${ARG_API})
		message(FATAL_ERROR "${ARG_API} not found!")
	endif()

	get_filename_component(apiFileName ${ARG_API} NAME_WLE)
	set(AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/autogen)
	set(HEADER ${AUTOGEN_BUILD_DIR}/${apiFileName}Skeleton.h)
	set(CORE ${AUTOGEN_BUILD_DIR}/${apiFileName}Skeleton.cpp)

	foreach (_file ${HEADER} ${CORE})
		if (NOT EXISTS ${_file})
			set(_execute_script 1)
		endif()
	endforeach()

	get_property(PY_IMPL_SCRIPT GLOBAL PROPERTY PY_IMPL_SCRIPT_PROPERTY)
	set(PY_CMD ${PY_IMPL_SCRIPT} --outputdir ${AUTOGEN_BUILD_DIR} ${ARG_API})
	add_custom_command(
	OUTPUT
		${HEADER}
		${CORE}
	DEPENDS
		${ARG_API}
	COMMAND ${Python3_EXECUTABLE} ${PY_CMD})

	if (_execute_script)
		execute_process(COMMAND ${Python3_EXECUTABLE} ${PY_CMD})
	endif()

	target_sources(${ARG_TARGET} PUBLIC ${HEADER} PRIVATE ${CORE})
	target_include_directories(${ARG_TARGET} PUBLIC ${AUTOGEN_BUILD_DIR})
endfunction()


####################################################################
# Python setup
####################################################################
osbase_service_install_python_packages(
	PACKAGES pyyaml
)
