set(DATA ${OSBASE}.data)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${DATA}.${UNIT_TEST} ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${DATA}.${UNIT_TEST} 
					  PRIVATE 
						${DATA} 
						${OSBASE}.data.impl
						${OSBASE}.core
						${OSBASE}.core.impl
					  )
target_include_directories(${DATA}.${UNIT_TEST} PRIVATE ${INCLUDE_DIR})
target_compile_definitions(${DATA}.${UNIT_TEST} PUBLIC TIMEOUT_FACTOR=1)

add_UT(UT ${DATA}.${UNIT_TEST})
