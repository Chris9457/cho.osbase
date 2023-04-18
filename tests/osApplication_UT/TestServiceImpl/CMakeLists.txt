set(TESTSERVICEIMPL ${APPLICATION}.${UNIT_TEST}.TestService.impl)

set(INCLUDE_DIR include)
set(BASE_INCLUDE_DIR ../include)
set(SRC_DIR src)

file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE BASE_INCLUDE_FILES ${BASE_INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${TESTSERVICEIMPL} STATIC ${BASE_INCLUDE_FILES} ${INCLUDE_FILES} ${SRC_FILES})

target_include_directories(${TESTSERVICEIMPL} PUBLIC ${INCLUDE_DIR} ${BASE_INCLUDE_DIR})
target_link_libraries(${TESTSERVICEIMPL} PRIVATE 
					  ${APPLICATION}
					  ${APPLICATION}.${UNIT_TEST}.TestService.api
					  )


set(API_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/../TestService/testservice.yml)

osbase_service_generate_impl_skeleton(
    TARGET ${TESTSERVICEIMPL}
	API ${API_INPUT}
)

