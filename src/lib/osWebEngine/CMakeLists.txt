set(WEBENGINE ${OSBASE}.webengine)

set(INCLUDE_DIR include)
set(SRC_DIR src)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB SRC_FILES ${SRC_DIR}/*)

add_library(${WEBENGINE} ${INCLUDE_FILES} ${SRC_FILES})

find_library(GDIPLUS_LIBRARY NAMES libgdiplus gdiplus)
set(GDIPLUS_LIBRARY gdiplus)

find_package(unofficial-webview2 CONFIG REQUIRED)
target_link_libraries(${WEBENGINE} 
					  PUBLIC 
					    ${OSBASE}.data
						${OSBASE}.core
					  PRIVATE
						${GDIPLUS_LIBRARY}
						unofficial::webview2::webview2
					 )
target_include_directories(${WEBENGINE} PUBLIC ${INCLUDE_DIR})
set_source_files_properties(${SRC_DIR}/CaptureWindowManager.cpp PROPERTIES COMPILE_FLAGS "-wd4458")

