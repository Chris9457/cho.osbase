set(WEBVIEWERAPI ${OS_WEBVIEWER}.api)

add_library(${WEBVIEWERAPI} ${OS_WEBVIEWER_API})
target_link_libraries(${WEBVIEWERAPI} PUBLIC ${OSBASE}.application)

osbase_service_generate_stub(
	TARGET ${WEBVIEWERAPI}
	API ${OS_WEBVIEWER_API}
)

