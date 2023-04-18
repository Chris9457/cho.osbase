set(CORE_IMPL ${OSBASE}.core.impl)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${CORE_IMPL} ${INCLUDE_FILES} ${SRC_FILES})

find_package(RapidJSON REQUIRED)
find_path(RAPIDXML_INCLUDE_DIRS "rapidxml/rapidxml.hpp")
target_include_directories(${CORE_IMPL}  PRIVATE ${RAPIDXML_INCLUDE_DIRS})
target_link_libraries(${CORE_IMPL} 
					  PRIVATE 
						${OSBASE}.core
						rapidjson
					  )
target_include_directories(${CORE_IMPL} PUBLIC ${INCLUDE_DIR})
target_compile_options(${CORE_IMPL} 
	PUBLIC 
		$<$<PLATFORM_ID:Windows>:/D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING>
		)
