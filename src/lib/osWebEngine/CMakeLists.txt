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

#if (WIN32)
#	find_program(NUGET_EXE 
#				 NAMES nuget
#				 PATHS ${ROOT_DIR}/tools)
#	if (NOT NUGET_EXE)
#		message("NUGET.EXE not found.")
#		message(FATAL_ERROR "Please install this executable, and run CMake again.")
#	endif()
#
#	set(WEBVIEW2_VERSION "1.0.1418.22")
#	set(IMPLEMENTATION_LIBRAY_VERSION "1.0.220914.1")
#
#	exec_program(${NUGET_EXE}
#    	ARGS install "Microsoft.Web.WebView2" -Version ${WEBVIEW2_VERSION} -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
#	exec_program(${NUGET_EXE}
#    	ARGS install "Microsoft.Windows.ImplementationLibrary" -Version ${IMPLEMENTATION_LIBRAY_VERSION} -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
#
#    target_link_libraries(${WEBENGINE} INTERFACE Version.lib Shlwapi.lib ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/x64/WebView2LoaderStatic.lib)
#    target_include_directories(${WEBENGINE} SYSTEM PUBLIC ${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/include ${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.ImplementationLibrary/include)
#endif()
