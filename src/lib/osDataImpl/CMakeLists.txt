set(DATA_IMPL ${OSBASE}.data.impl)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${DATA_IMPL} ${INCLUDE_FILES} ${SRC_FILES})

find_package(wampcc CONFIG REQUIRED)
find_package(websocketpp CONFIG REQUIRED)

target_link_libraries(${DATA_IMPL}
					  PRIVATE
					  	${OSBASE}.data
					  	wampcc::wampcc_static
					  	wampcc::wampcc_json_static
					  	websocketpp::websocketpp
					  )

target_include_directories(${DATA_IMPL} PUBLIC ${INCLUDE_DIR})

target_compile_options(${DATA_IMPL}
	PUBLIC
		$<$<PLATFORM_ID:Windows>:/DWIN32_LEAN_AND_MEAN>
		$<$<PLATFORM_ID:Windows>:/D_WIN32_WINNT=0x0601>
		$<$<PLATFORM_ID:Windows>:/D_WINSOCK_DEPRECATED_NO_WARNINGS>
		)

