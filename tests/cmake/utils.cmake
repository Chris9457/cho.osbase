####################################################################################################
# add_UT: add a unit tests suite
#	- [in]			UT		=> name of the test unit
#	- [in - opt]	SRC_DIR	=> list of the directories of the tested libraries
#
function(add_UT)
	set(options "")
	set(oneValueArgs UT)
	set(multiValueArgs SRC_DIR)
	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	
	##########################################
	# check mandatory params
	foreach(arg UT SRC_DIR)
		if (NOT ARG_${arg})
			list(APPEND missing_params ${arg})
		endif()
	endforeach()
	
	if (missing_params)
		message(FATAL_ERROR "add_UT: missing params => ${missing_params}")
	endif()
	##########################################

	find_package(GTest REQUIRED)
	target_link_libraries(${ARG_UT} PRIVATE GTest::GTest)

    set(XML_OUTDIR ${CMAKE_BINARY_DIR}/gtest)
	file(MAKE_DIRECTORY ${XML_OUTDIR})
	set(XML_LIST_OUTDIR ${CMAKE_BINARY_DIR}/gtest-list)
    file(MAKE_DIRECTORY ${XML_LIST_OUTDIR})

	add_test(NAME ${ARG_UT}
				COMMAND ${ARG_UT} --gtest_output=xml:${XML_OUTDIR}/${ARG_UT}.xml
				WORKING_DIRECTORY $<TARGET_FILE_DIR:${ARG_UT}>)

	set_tests_properties(${ARG_UT} PROPERTIES TIMEOUT 120)

endfunction()

