# This script contains functions that are likely to be usefull to many C++ projects

include(GNUInstallDirs)
include(${CMAKE_SOURCE_DIR}/src/lib/osApplication/script/osApplication.cmake)

set(DOWNLOADED_DIR ${CMAKE_SOURCE_DIR}/downloaded)

# Downloads a zip files and uncompres it to ${DOWNLOADED_DIR}/${OUTPUT_DIR} if not already done
#Example: download_zip_package(SOURCE_FILES "file_to_download1" "file_to_download2" OUTPUT_DIR "output_folder")
function(download_zip_package)
	set(options DONT_CHECK_OUPUT_DIR_EXIST)
	set(oneValueArgs OUTPUT_DIR)
	set(multiValueArgs SOURCE_FILES)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg OUTPUT_DIR SOURCE_FILES)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "download_zip_package: missing params => ${missing_params}")
	endif()
	##########################################

    set(_localdir ${DOWNLOADED_DIR}/${ARG_OUTPUT_DIR})
    if (NOT ARG_DONT_CHECK_OUPUT_DIR_EXIST AND EXISTS "${_localdir}")
        message("Package '${ARG_OUTPUT_DIR}' already found in ${_localdir}")
        return()
    endif()

	file(REMOVE_RECURSE ${_localdir})
	foreach (_pkg_url ${ARG_SOURCE_FILES})
		message("Downloading package '${ARG_OUTPUT_DIR}' from ${_pkg_url}")
		set(_tmp_pkg_path ${CMAKE_BINARY_DIR}/dowloaded.zip)
		file(DOWNLOAD ${_pkg_url} ${_tmp_pkg_path} SHOW_PROGRESS TIMEOUT 900 STATUS _download_status)
		list(GET _download_status 0 _download_status_code)
		list(GET _download_status 1 _download_status_msg)
		if (NOT _download_status_code EQUAL 0)
			message(FATAL_ERROR "Error: failed to download package '${ARG_OUTPUT_DIR}': ${_download_status_msg}")
		endif()

		message("Uncompressing package to ${_localdir}")
		file(ARCHIVE_EXTRACT INPUT ${_tmp_pkg_path} DESTINATION ${_localdir})
		file(REMOVE ${_tmp_pkg_path})
	endforeach()
endfunction()

function(install_deps)
	set(options "")
	set(oneValueArgs TARGET DESTINATION)
	set(multiValueArgs "")
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg TARGET DESTINATION)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "install_deps: missing params => ${missing_params}")
	endif()
	##########################################

	find_program(POWERSHELL powershell)

	add_custom_command(TARGET ${ARG_TARGET} POST_BUILD
					   COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_DESTINATION}"
					   COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_DESTINATION}"
					   COMMAND ${POWERSHELL} -file "${CMAKE_SOURCE_DIR}/cmake/install_deps.ps1" 
								-SourcePath "$<TARGET_FILE_DIR:${ARG_TARGET}>" 
								-DestinationPath "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_DESTINATION}"
								-FileExtension ${CMAKE_SHARED_LIBRARY_SUFFIX}
					   )
	install(DIRECTORY "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_DESTINATION}" DESTINATION .)
endfunction()

function(copy_executable)
	set(options "")
	set(oneValueArgs TARGET EXE)
	set(multiValueArgs "")
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg TARGET EXE)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "copy_executable: missing params => ${missing_params}")
	endif()
	##########################################

	find_program(POWERSHELL powershell)
	add_custom_command(TARGET ${ARG_TARGET} POST_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${ARG_EXE}>" "$<TARGET_FILE_DIR:${ARG_TARGET}>"
					   COMMAND ${POWERSHELL} -file "${CMAKE_SOURCE_DIR}/cmake/install_deps.ps1" 
								-SourcePath "$<TARGET_FILE_DIR:${ARG_EXE}>" 
								-DestinationPath "$<TARGET_FILE_DIR:${ARG_TARGET}>"
								-FileExtension ${CMAKE_SHARED_LIBRARY_SUFFIX}
					  )
endfunction()