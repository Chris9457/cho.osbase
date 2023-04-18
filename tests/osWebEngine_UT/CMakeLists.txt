set(WEBENGINE ${OSBASE}.webengine)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${WEBENGINE}.${UNIT_TEST} ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${WEBENGINE}.${UNIT_TEST} 
					  PRIVATE 
						${WEBENGINE} 
					  )
target_include_directories(${WEBENGINE}.${UNIT_TEST} PRIVATE ${INCLUDE_DIR})

add_UT(
	UT ${WEBENGINE}.${UNIT_TEST}
	SRC_DIR ${CMAKE_SOURCE_DIR}/src/osWebEngine
)
