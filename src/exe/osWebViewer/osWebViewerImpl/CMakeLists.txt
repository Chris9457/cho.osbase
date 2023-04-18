set(WEBVIEWERIMPL ${OS_WEBVIEWER}.impl)

set(INCLUDE_DIR include)
set(SRC_DIR src)
set(RESOURCES_DIR resources)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)
file(GLOB RESOURCES_FILES ${RESOURCES_DIR}/*)

if (NOT OS_WEBVIEWER_SETTING_FILENAME)
	set(OS_WEBVIEWER_SETTING_FILENAME "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCES_DIR}/settings.json")
endif()

get_filename_component(setting_file_name ${OS_WEBVIEWER_SETTING_FILENAME} NAME)
set(SETTING_FILE_NAME ${RESOURCES_DIR}/${setting_file_name})
configure_file(${OS_WEBVIEWER_SETTING_FILENAME} ${CMAKE_CURRENT_BINARY_DIR}/${SETTING_FILE_NAME})

add_library(${WEBVIEWERIMPL} ${INCLUDE_FILES} ${SRC_FILES} ${RESOURCES_FILES})
target_link_libraries(${WEBVIEWERIMPL} 
					  PUBLIC
						${OSBASE}.application
					    ${OSBASE}.webengine
					    ${OS_WEBVIEWER}.api
					 )
target_include_directories(${WEBVIEWERIMPL} PUBLIC ${INCLUDE_DIR})
target_compile_definitions(${WEBVIEWERIMPL} PRIVATE SETTING_FILE_NAME="${SETTING_FILE_NAME}")

osbase_service_generate_impl_skeleton(
	TARGET ${WEBVIEWERIMPL}
	API ${OS_WEBVIEWER_API}
)
