set(CORE ${OSBASE}.core)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_subdirectory(Static)
add_subdirectory(Shared)

add_executable(${CORE}.${UNIT_TEST} ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${CORE}.${UNIT_TEST} 
        			  PRIVATE 
					  	${CORE}.${UNIT_TEST}.shared
					  	${CORE}.${UNIT_TEST}.static
					  	${CORE} 
					  	${CORE}.impl
					  )
target_include_directories(${CORE}.${UNIT_TEST} PRIVATE ${INCLUDE_DIR})

add_custom_command(TARGET ${CORE}.${UNIT_TEST} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${CORE}.${UNIT_TEST}.shared> $<TARGET_FILE_DIR:${CORE}.${UNIT_TEST}>
				   )
if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
	add_custom_command(TARGET ${CORE}.${UNIT_TEST} POST_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${CORE}.${UNIT_TEST}.shared> $<TARGET_FILE_DIR:${CORE}.${UNIT_TEST}>
					   )
endif()

add_UT(UT ${CORE}.${UNIT_TEST})
 