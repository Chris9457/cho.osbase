set(STATEMACHINE ${OSBASE}.statemachine)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${STATEMACHINE}.${UNIT_TEST} ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${STATEMACHINE}.${UNIT_TEST} 
					  PRIVATE 
						${STATEMACHINE} 
						${OSBASE}.data.impl
						${OSBASE}.core
						${OSBASE}.core.impl
					  )
target_include_directories(${STATEMACHINE}.${UNIT_TEST} PRIVATE ${INCLUDE_DIR})

add_UT(
	UT ${STATEMACHINE}.${UNIT_TEST}
	SRC_DIR ${CMAKE_SOURCE_DIR}/src/osStateMachine
)
