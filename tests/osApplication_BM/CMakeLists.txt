set(APPLICATION ${OSBASE}.application)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)
file(GLOB YML_FILES *.yml)

set(SERVICE_BM_API ${CMAKE_CURRENT_BINARY_DIR}/service_bm.yml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/service_bm.yml.in ${SERVICE_BM_API})

add_executable(${APPLICATION}.${BENCHMARK} ${INCLUDE_FILES} ${SRC_FILES} ${YML_FILES})

find_package(benchmark REQUIRED)
target_link_libraries(${APPLICATION}.${BENCHMARK} 
					  PRIVATE 
						${APPLICATION} 
						${APPLICATION}.${UNIT_TEST}.TestService.api
						${APPLICATION}.${UNIT_TEST}.TestService.impl
						${OSBASE}.core.impl
						${OSBASE}.data.impl
						benchmark::benchmark
					  )
target_include_directories(${APPLICATION}.${BENCHMARK} PRIVATE ${INCLUDE_DIR})

osbase_service_generate_stub(TARGET ${APPLICATION}.${BENCHMARK} API ${SERVICE_BM_API})
osbase_service_generate_impl_skeleton(TARGET ${APPLICATION}.${BENCHMARK} API ${SERVICE_BM_API})
