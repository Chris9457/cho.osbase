set(BROKER ${OSBASE}.broker)

set(SRC_DIR src)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${BROKER} ${INCLUDE_FILES} ${SRC_FILES})

target_link_libraries(${BROKER} PRIVATE
					  ${OSBASE}.application
					  ${OSBASE}.core
					  ${OSBASE}.data.impl
					 )

install(TARGETS ${BROKER} DESTINATION broker)
install(FILES $<TARGET_RUNTIME_DLLS:${BROKER}> DESTINATION broker)
