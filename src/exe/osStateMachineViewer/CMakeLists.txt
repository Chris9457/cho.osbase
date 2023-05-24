set(STATEMACHINEVIEWER ${OSBASE}.statemachineviewer)

set(SRC_DIR src)
set(RESOURCES_DIR resources)
file(GLOB SRC_FILES ${SRC_DIR}/*)
file(GLOB RESOURCES_FILES ${RESOURCES_DIR}/*)

find_package(Qt5 COMPONENTS Core Widgets Svg REQUIRED)

qt5_add_resources(QT_RESOURCES_FILE ${RESOURCES_DIR}/statemachineviewer.qrc)

set(AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/autogen_src)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(${STATEMACHINEVIEWER} ${SRC_FILES} ${QT_RESOURCES_FILE} ${RESOURCES_FILES})
target_link_libraries(${STATEMACHINEVIEWER} 
					  PRIVATE 
						${OSBASE}.log.api
						${OSBASE}.statemachine
						${OSBASE}.application
						${OSBASE}.core.impl
						${OSBASE}.data.impl
						Qt5::Widgets)

set_property(TARGET ${STATEMACHINEVIEWER} PROPERTY WIN32_EXECUTABLE true)
target_compile_definitions(${STATEMACHINEVIEWER} PRIVATE OSBASE=${OSBASE})

install(TARGETS ${STATEMACHINEVIEWER} DESTINATION statemachineviewer)
install(FILES $<TARGET_FILE_DIR:${STATEMACHINEVIEWER}>/qt.conf DESTINATION statemachineviewer)
install(DIRECTORY $<TARGET_FILE_DIR:${STATEMACHINEVIEWER}>/plugins DESTINATION statemachineviewer)
install_deps(TARGET ${STATEMACHINEVIEWER} DESTINATION statemachineviewer)