set(CORE ${OSBASE}.core)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${CORE} ${INCLUDE_FILES} ${SRC_FILES})

target_include_directories(${CORE} PUBLIC ${INCLUDE_DIR})
target_compile_options(${CORE} PUBLIC /W4 /WX /sdl /external:W3)
target_compile_definitions(${CORE} PUBLIC UNICODE _UNICODE)

# deactivate CRT_SECURE for specific sources
list(APPEND no_crt_secure_sources "${SRC_DIR}/StructuredExceptionHandler.cpp")
set_source_files_properties(${no_crt_secure_sources} PROPERTIES COMPILE_FLAGS /D_CRT_SECURE_NO_WARNINGS)