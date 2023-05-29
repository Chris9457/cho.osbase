set(DATA ${OSBASE}.data)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${DATA} ${INCLUDE_FILES} ${SRC_FILES})
target_link_libraries(${DATA} 
					  PUBLIC 
						${OSBASE}.core
					 )
target_include_directories(${DATA} PUBLIC ${INCLUDE_DIR})
target_compile_definitions(${DATA} PUBLIC OSBASE="${OSBASE}")

# deactivate CRT_SECURE for specific sources
list(APPEND no_crt_secure_sources "${SRC_DIR}/LoggerMessage.cpp")
set_source_files_properties(${no_crt_secure_sources} PROPERTIES COMPILE_FLAGS /D_CRT_SECURE_NO_WARNINGS)