set(LAUNCHER ${OSBASE}.launcher)

set(INCLUDE_DIR include)
set(SRC_DIR src)
set(RESOURCES_DIR resources)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)
file(GLOB_RECURSE RESOURCES_FILES ${RESOURCES_DIR}/*)

set(SETTING_FILE_NAME ${RESOURCES_DIR}/launcher.json)
configure_file(${SETTING_FILE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${SETTING_FILE_NAME})

add_executable(${LAUNCHER} ${INCLUDE_FILES} ${SRC_FILES} ${RESOURCES_FILES})

target_link_libraries(${LAUNCHER} PRIVATE
					  ${OSBASE}.application
					  ${OSBASE}.core
					  ${OSBASE}.core.impl
					  ${OSBASE}.data
					  ${OSBASE}.data.impl
					 )
target_include_directories(${LAUNCHER} PRIVATE
						   ${INCLUDE_DIR}
						  )
target_compile_definitions(${LAUNCHER} PRIVATE
						   SETTING_FILE_NAME="${SETTING_FILE_NAME}"
						   BROKER_NAME="../broker/${OSBASE}.broker.exe")

install(TARGETS ${LAUNCHER} DESTINATION launcher)
install(FILES $<TARGET_RUNTIME_DLLS:${LAUNCHER}> DESTINATION launcher)
install(DIRECTORY $<TARGET_FILE_DIR:${LAUNCHER}>/resources DESTINATION launcher)
