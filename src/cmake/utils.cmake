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
        DESTINATION ${CMAKE_INSTALL_BINDIR}/${ARG_OUTPUT_DIR}
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
                DESTINATION ${CMAKE_INSTALL_BINDIR}/${ARG_OUTPUT_DIR}/plugins/platforms
            )
        elseif (${_lowerPlugin} STREQUAL "imageformats")
            install(DIRECTORY ${_qt_bin_dir}/../plugins/imageformats
                DESTINATION ${CMAKE_INSTALL_BINDIR}/${ARG_OUTPUT_DIR}/plugins
            )
        else()
            message(FATAL_ERROR "install_qt_plugins: plugin not handled: " ${_plugin})
        endif()
    endforeach()
endfunction()

# Utility function to create a new Google Test
# example: add_google_test(MyTest test1.cpp test2.cpp)
function(add_google_test TargetName)
    add_executable(${TargetName} ${ARGN})
    target_precompile_headers(${TargetName} PRIVATE ${TargetName}.h)
    target_link_libraries(${TargetName} PRIVATE CONAN_PKG::gtest)

    set(_xml_outdir ${CMAKE_BINARY_DIR}/gtest)
    file(MAKE_DIRECTORY ${_xml_outdir})
    add_test(NAME ${TargetName} COMMAND ${TargetName} --gtest_output=xml:${_xml_outdir}/${_target_name}.xml)
    set_tests_properties(${TargetName} PROPERTIES TIMEOUT 120)
endfunction()

# Utility function to install a software unit
# example: install_software_unit(${SOFWARE_UNIT})
function(install_software_unit)
    foreach (_sw_unit ${ARGN})
        set(_dest_dir ${CMAKE_INSTALL_BINDIR}/${_sw_unit})
        install(TARGETS ${_sw_unit} DESTINATION ${_dest_dir})
		string(COMPARE EQUAL CMAKE_BUILD_TYPE "Release" Release)
        if (NOT Release)
	        install(FILES $<TARGET_PDB_FILE:${_sw_unit}> DESTINATION ${_dest_dir})
        endif()
    endforeach()
endfunction()

# Utility function to add a new configuration file
# example: add_configuration_file(TAGNAME myTag INPUT_FILE inFile OUTPUT_FILE outFile)
function(add_configuration_file)
	set(options COPYONLY)
	set(oneValueArgs TAGNAME INPUT_FILE OUTPUT_FILE)
	set(multiValueArgs "")
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg TAGNAME INPUT_FILE)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "add_configuration_file: missing params => ${missing_params}")
	endif()
	##########################################

	if (NOT ARG_OUTPUT_FILE)
		set(ARG_OUTPUT_FILE ${ARG_INPUT_FILE})
	endif()

	set(OUTPUT_SETTINGS_DIR ${CMAKE_CURRENT_BINARY_DIR}/settings)
	if (ARG_COPYONLY)
		configure_file(${ARG_INPUT_FILE} ${OUTPUT_SETTINGS_DIR}/${ARG_OUTPUT_FILE} COPYONLY)
	else()
		configure_file(${ARG_INPUT_FILE} ${OUTPUT_SETTINGS_DIR}/${ARG_OUTPUT_FILE})
	endif()

	get_property(conf_files GLOBAL PROPERTY ${ARG_TAGNAME}_CONFIGURATION)
	set(conf_files ${conf_files} ${OUTPUT_SETTINGS_DIR}/)
	list(REMOVE_DUPLICATES conf_files)
	set_property(GLOBAL PROPERTY ${ARG_TAGNAME}_CONFIGURATION ${conf_files})
endfunction()

# Utility function to install configuration files for a target
# example: install_configuration_files(TAGNAME myTag)
function(install_configuration_files)
	set(options "")
	set(oneValueArgs TAGNAME SOFTWARE_UNIT)
	set(multiValueArgs "")
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg TAGNAME)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "install_configuration_files: missing params => ${missing_params}")
	endif()
	##########################################

	get_property(conf_directories GLOBAL PROPERTY ${ARG_TAGNAME}_CONFIGURATION)
	if (conf_directories)
		if (ARG_SOFTWARE_UNIT)
			install(DIRECTORY ${conf_directories} DESTINATION ${CMAKE_INSTALL_BINDIR}/${SOFTWARE_UNIT})
		else ()
			install(DIRECTORY ${conf_directories} DESTINATION ${CMAKE_INSTALL_BINDIR})
		endif ()
	endif()
endfunction()

