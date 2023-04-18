set(OS_LOG ${OSBASE}.log)
set(OS_LOG_API ${CMAKE_CURRENT_SOURCE_DIR}/osLog.yml)

add_subdirectory(osLogApi)
add_subdirectory(osLogImpl)
add_subdirectory(osLogService)