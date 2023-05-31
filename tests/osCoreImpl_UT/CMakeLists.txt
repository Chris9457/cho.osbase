set(CORE_IMPL ${OSBASE}.core.impl)

set(INCLUDE_DIR include)
set(SRC_DIR src)
set(RESOURCES_DIR resources)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)
file(GLOB_RECURSE RESOURCES_FILES ${RESOURCES_DIR}/*)

add_executable(${CORE_IMPL}.${UNIT_TEST} ${INCLUDE_FILES} ${SRC_FILES} ${RESOURCES_FILES})
target_link_libraries(${CORE_IMPL}.${UNIT_TEST} 
					  PRIVATE 
						${CORE_IMPL}
						${OSBASE}.core
					  )
target_include_directories(${CORE_IMPL}.${UNIT_TEST} 
						   PRIVATE 
							${INCLUDE_DIR}
							../osCore_UT/include)

add_custom_command(TARGET ${CORE_IMPL}.${UNIT_TEST} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCES_DIR} $<TARGET_FILE_DIR:${CORE_IMPL}.${UNIT_TEST}>/${RESOURCES_DIR})

add_UT(UT ${CORE_IMPL}.${UNIT_TEST})

 