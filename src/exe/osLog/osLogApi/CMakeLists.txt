set(LOGAPI ${OS_LOG}.api)

add_library(${LOGAPI} ${OS_LOG_API})
target_link_libraries(${LOGAPI} PUBLIC ${OSBASE}.application)

OSBASE_SERVICE_GENERATE_STUB(
	TARGET ${LOGAPI}
	API ${OS_LOG_API}
)