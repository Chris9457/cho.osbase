set(OS_WEBVIEWER ${OSBASE}.webviewer)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/osWebViewer.yml.in ${CMAKE_CURRENT_BINARY_DIR}/osWebViewer.yml)
set(OS_WEBVIEWER_API ${CMAKE_CURRENT_BINARY_DIR}/osWebViewer.yml)

add_subdirectory(osWebViewerApi)
add_subdirectory(osWebViewerImpl)
add_subdirectory(osWebViewerService)
