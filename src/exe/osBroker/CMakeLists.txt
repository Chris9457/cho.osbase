set(BROKER ${OSBASE}.broker)

set(SRC_DIR src)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${BROKER} ${INCLUDE_FILES} ${SRC_FILES})

target_link_libraries(${BROKER} PRIVATE
					  ${OSBASE}.application
					  ${OSBASE}.core
					  ${OSBASE}.core.impl
					  ${OSBASE}.data.impl
					 )

install(TARGETS ${BROKER} DESTINATION broker)
install_deps(TARGET ${BROKER} DESTINATION broker)
