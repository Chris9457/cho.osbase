set(WEBVIEWER ${OSBASE}.webviewer)

set(SRC_DIR src)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${WEBVIEWER}.${UNIT_TEST} ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${WEBVIEWER}.${UNIT_TEST} 
					  PRIVATE 
						${OSBASE}.webviewer.api
						${OSBASE}.core.impl
						${OSBASE}.data.impl
					  )
target_include_directories(${WEBVIEWER}.${UNIT_TEST} PRIVATE ${INCLUDE_DIR})

target_compile_definitions(${WEBVIEWER}.${UNIT_TEST} PRIVATE
						   WEBVIEWER_NAME="${WEBVIEWER}.service.exe"
						   )

file(GLOB DEPENDENCIES_DLL ${CMAKE_CURRENT_BINARY_DIR}/*.dll)

add_custom_command(
	TARGET ${WEBVIEWER}.${UNIT_TEST} POST_BUILD
	COMMAND_EXPAND_LISTS
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${WEBVIEWER}.service> $<TARGET_FILE_DIR:${WEBVIEWER}.${UNIT_TEST}>/$<TARGET_FILE_NAME:${WEBVIEWER}.service>
	COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${WEBVIEWER}.${UNIT_TEST}>  $<TARGET_RUNTIME_DLLS:${WEBVIEWER}.service>
	COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:${WEBVIEWER}.impl>/resources $<TARGET_FILE_DIR:${WEBVIEWER}.${UNIT_TEST}>/resources
)

add_UT(
	UT ${WEBVIEWER}.${UNIT_TEST}
	SRC_DIR ${CMAKE_SOURCE_DIR}/src/osWebViewer
)
