include(script/osApplication.cmake)

set(APPLICATION ${OSBASE}.application)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)
file(GLOB DOC_FILES ./*.md)

add_library(${APPLICATION} ${INCLUDE_FILES} ${SRC_FILES} ${DOC_FILES})

target_include_directories(${APPLICATION} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${APPLICATION} 
					  PUBLIC 
						${OSBASE}.core
						${OSBASE}.data
					 )

#target_compile_definitions(${APPLICATION} PUBLIC CHO_OSBASE_APPLICATION_SERVICE_TRACE)

