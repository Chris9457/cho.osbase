set(STATEMACHINE ${OSBASE}.statemachine)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${STATEMACHINE} ${INCLUDE_FILES} ${SRC_FILES} ${STATEMACHINE}.natvis)
target_link_libraries(${STATEMACHINE} 
					  PUBLIC 
						${OSBASE}.core
						${OSBASE}.application
					 )
target_include_directories(${STATEMACHINE} PUBLIC ${INCLUDE_DIR})
