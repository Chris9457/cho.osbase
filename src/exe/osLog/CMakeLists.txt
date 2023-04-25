set(OS_LOG ${OSBASE}.log)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osLog.yml.in ${CMAKE_CURRENT_BINARY_DIR}/osLog.yml)
set(OS_LOG_API ${CMAKE_CURRENT_BINARY_DIR}/osLog.yml)

add_subdirectory(osLogApi)
add_subdirectory(osLogImpl)
add_subdirectory(osLogService)