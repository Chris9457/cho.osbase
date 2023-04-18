set(LOGSERVICE ${OS_LOG}.service)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${LOGSERVICE} ${INCLUDE_FILES} ${SRC_FILES})

target_link_libraries(${LOGSERVICE} PRIVATE
					  ${OS_LOG}.impl
					  ${OS_LOG}.api
					  ${OSBASE}.application
					  ${OSBASE}.core
					  ${OSBASE}.core.impl
					  ${OSBASE}.data.impl
					 )
target_include_directories(${LOGSERVICE} PRIVATE ${INCLUDE_DIR})

install(TARGETS ${LOGSERVICE} DESTINATION log)
install(FILES $<TARGET_RUNTIME_DLLS:${LOGSERVICE}> DESTINATION log)
install(DIRECTORY $<TARGET_FILE_DIR:${OS_LOG}.impl>/resources DESTINATION log)
