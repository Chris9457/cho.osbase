set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)


add_executable(${BROKER} ${INCLUDE_FILES} ${INCLUDE_FILES} ${SRC_FILES})

target_link_libraries(${BROKER} PRIVATE
					  ${BROKER}.api
					  ${OSBASE}.application
					  ${OSBASE}.core
					  ${OSBASE}.core.impl
					  ${OSBASE}.data.impl
					 )

target_include_directories(${BROKER} PUBLIC ${INCLUDE_DIR})

install(TARGETS ${BROKER} DESTINATION broker)
install_deps(TARGET ${BROKER} DESTINATION broker)
