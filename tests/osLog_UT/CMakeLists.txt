set(LOG ${OSBASE}.log)

set(SRC_DIR src)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_executable(${LOG}.${UNIT_TEST} ${SRC_FILES})

target_link_libraries(${LOG}.${UNIT_TEST} 
					  PRIVATE 
					    ${LOG}.api
						${OSBASE}.core.impl
						${OSBASE}.data.impl
					  )

target_compile_definitions(${LOG}.${UNIT_TEST} PRIVATE 
						   LOG_NAME="${LOG}.service.exe"
						   )

add_custom_command(
	TARGET ${LOG}.${UNIT_TEST} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${LOG}.impl>/resources $<TARGET_FILE_DIR:${LOG}.${UNIT_TEST}>/resources
)

copy_executable(TARGET ${LOG}.${UNIT_TEST}
				EXE ${LOG}.service
				DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_UT(UT ${LOG}.${UNIT_TEST})