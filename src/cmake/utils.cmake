# This script contains functions that are likely to be usefull to many C++ projects

include(GNUInstallDirs)
include(lib/osApplication/script/osApplication.cmake)

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

function(get_qt_bin_dir)
	set(options "")
	set(oneValueArgs OUTPUT_QT_BIN_DIR)
	set(multiValueArgs "")
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg OUTPUT_QT_BIN_DIR)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "get_qt_bin_dir: missing params => ${missing_params}")
	endif()

	##########################################
    # locate Qt5
    find_package(Qt5Core REQUIRED)
    get_target_property(_qt_moc_path Qt5::moc IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qt_moc_path}" DIRECTORY)

    set(${ARG_OUTPUT_QT_BIN_DIR} ${_qt_bin_dir} PARENT_SCOPE)
endfunction()

function(get_qt_dlls)
	set(options VERBOSE)
	set(oneValueArgs OUTPUT_QT_DLL)
	set(multiValueArgs DLL_FILES)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg DLL_FILES OUTPUT_QT_DLL)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "get_qt_dlls: missing params => ${missing_params}")
	endif()

    get_qt_bin_dir(OUTPUT_QT_BIN_DIR _qt_bin_dir)

    # process the given list of dlls
	if (${ARG_VERBOSE})
	    message("Adding copy instructions for Qt5 dlls")
	endif()
    foreach(_dll_file ${ARG_DLL_FILES})
		if (CMAKE_BUILD_TYPE STREQUAL "Debug")
			set(_dll_suffix d)
		endif()
		set(_dll_file ${_dll_file}${_dll_suffix}.dll)

        set(_dll_path "${_qt_bin_dir}/${_dll_file}")
        if (NOT EXISTS "${_dll_path}")
            message(FATAL_ERROR "Failed to find ${_dll_path}")
        endif()

		if (${ARG_VERBOSE})
		    message("-- Found ${_dll_path}")
		endif()
        list(APPEND _dll_files ${_dll_path})
    endforeach()

    set(${ARG_OUTPUT_QT_DLL} ${_dll_files} PARENT_SCOPE)

endfunction()

# Adds install() instructions for the given list of Qt5 dlls
# Example: install_qt5_dlls(Qt5Core Qt5Gui Qt5Widgets)
function(install_qt5_dlls)
	set(options VERBOSE)
	set(oneValueArgs OUTPUT_DIR)
	set(multiValueArgs DLL_FILES)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg DLL_FILES OUTPUT_DIR)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "install_qt5_dlls: missing params => ${missing_params}")
	endif()

    get_qt_bin_dir(OUTPUT_QT_BIN_DIR _qt_bin_dir)

    # process the given list of dlls
	if (${ARG_VERBOSE})
	    message("Adding install instructions for Qt5 dlls")
	endif()
    get_qt_dlls(DLL_FILES ${ARG_DLL_FILES} OUTPUT_QT_DLL _qt5_dlls_to_install)

    # install Qt dlls
    install(FILES ${_qt5_dlls_to_install}
        DESTINATION ${ARG_OUTPUT_DIR}
    )
endfunction()

# Adds install() instructions for the Qt plugins
function(install_qt_plugins)
	set(options "")
	set(oneValueArgs OUTPUT_DIR)
	set(multiValueArgs PLUGINS)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg OUTPUT_DIR PLUGINS)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "install_qt_plugins: missing params => ${missing_params}")
	endif()

    get_qt_bin_dir(OUTPUT_QT_BIN_DIR _qt_bin_dir)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_debugSuffix d)
    endif()

    foreach (_plugin ${ARG_PLUGINS})
        string(TOLOWER ${_plugin} _lowerPlugin)
        if (${_lowerPlugin} STREQUAL "platform")
            install(FILES ${_qt_bin_dir}/../plugins/platforms/qwindows${_debugSuffix}.dll
                DESTINATION ${ARG_OUTPUT_DIR}/plugins/platforms
            )
        elseif (${_lowerPlugin} STREQUAL "imageformats")
            install(DIRECTORY ${_qt_bin_dir}/../plugins/imageformats
                DESTINATION ${ARG_OUTPUT_DIR}/plugins
            )
        else()
            message(FATAL_ERROR "install_qt_plugins: plugin not handled: " ${_plugin})
        endif()
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
					   COMMAND ${POWERSHELL} -file "${CMAKE_SOURCE_DIR}/src/cmake/install_deps.ps1" 
								-SourcePath "$<TARGET_FILE_DIR:${ARG_TARGET}>" 
								-DestinationPath "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_DESTINATION}"
								-FileExtension ${CMAKE_SHARED_LIBRARY_SUFFIX}
					   )

	install(DIRECTORY "$<TARGET_FILE_DIR:${ARG_TARGET}>/${ARG_DESTINATION}" DESTINATION .)
endfunction()

