set(CORE ${OSBASE}.core)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)

add_executable(${CORE}.${BENCHMARK} ${INCLUDE_FILES} ${SRC_FILES})

find_package(benchmark REQUIRED)
target_link_libraries(${CORE}.${BENCHMARK} 
					  PRIVATE 
						${CORE} 
						benchmark::benchmark
					  )
target_include_directories(${CORE}.${BENCHMARK} PRIVATE ${INCLUDE_DIR})

 