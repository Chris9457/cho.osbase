set(LOGVIEWER ${OSBASE}.logviewer)

set(INCLUDE_DIR include)
set(SOURCE_DIR src)
set(RESOURCES_DIR resources)
set(SETTING_FILE_NAME ${RESOURCES_DIR}/logviewersettings.json)

file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SOURCE_FILES ${SOURCE_DIR}/*)
file(GLOB RESOURCES_FILES ${RESOURCES_DIR}/*)

configure_file(${SETTING_FILE_NAME}.in ${CMAKE_CURRENT_BINARY_DIR}/${SETTING_FILE_NAME})

add_executable(${LOGVIEWER} ${INCLUDE_FILES} ${SOURCE_FILES} ${RESOURCES_FILES})

target_link_libraries(${LOGVIEWER} PRIVATE 
					  ${OSBASE}.log.api
					  ${OSBASE}.application
					  ${OSBASE}.core
					  ${OSBASE}.core.impl
					  ${OSBASE}.data
					  ${OSBASE}.data.impl
					 )
target_include_directories(${LOGVIEWER} PRIVATE ${INCLUDE_DIR})
target_compile_definitions(${LOGVIEWER} PRIVATE SETTING_FILE_NAME="${SETTING_FILE_NAME}")

install(TARGETS ${LOGVIEWER} DESTINATION logviewer)
install(DIRECTORY $<TARGET_FILE_DIR:${LOGVIEWER}>/${RESOURCES_DIR} DESTINATION logviewer)
install_deps(TARGET ${LOGVIEWER} DESTINATION logviewer)