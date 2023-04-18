set(LIB_SHARED ${CORE}.${UNIT_TEST}.shared)

set(INCLUDE_DIR include)
set(BASE_INCLUDE_DIR ../include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE BASE_INCLUDE_FILES ${BASE_INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${LIB_SHARED} SHARED ${BASE_INCLUDE_FILES} ${INCLUDE_FILES} ${SRC_FILES})

target_link_libraries(${LIB_SHARED} 
                       PRIVATE 
                        ${CORE}.${UNIT_TEST}.static 
                        ${CORE})

target_include_directories(${LIB_SHARED} PUBLIC ${INCLUDE_DIR} ${BASE_INCLUDE_DIR})
target_compile_definitions(${LIB_SHARED} PRIVATE DLL_EXPORT)
