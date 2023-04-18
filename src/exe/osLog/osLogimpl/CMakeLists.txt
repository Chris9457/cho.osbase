set(LOGIMPL ${OS_LOG}.impl)

set(INCLUDE_DIR include)
set(SRC_DIR src)
set(RESOURCES_DIR resources)

file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)
file(GLOB RESOURCES_FILES ${RESOURCES_DIR}/*)

set(SETTING_FILE_NAME ${RESOURCES_DIR}/logsettings.json)
configure_file(${SETTING_FILE_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${SETTING_FILE_NAME})

add_library(${LOGIMPL} STATIC ${INCLUDE_FILES} ${SRC_FILES} ${RESOURCES_FILES})

target_include_directories(${LOGIMPL} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${LOGIMPL} PRIVATE 
					  ${OS_LOG}.api
					  ${OSBASE}.application
					  ${OSBASE}.core)
set_source_files_properties(${SRC_DIR}/LogServiceImpl.cpp PROPERTIES COMPILE_FLAGS -D_CRT_SECURE_NO_WARNINGS)
target_compile_definitions(${LOGIMPL} PRIVATE SETTING_FILE_NAME="${SETTING_FILE_NAME}")

OSBASE_SERVICE_GENERATE_IMPL_SKELETON(
    TARGET ${LOGIMPL}
	API ${OS_LOG_API}
)

