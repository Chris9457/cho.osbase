set(STATEMACHINEVIEWER ${OSBASE}.statemachineviewer)

set(SRC_DIR src)
set(RESOURCES_DIR resources)
file(GLOB SRC_FILES ${SRC_DIR}/*)
file(GLOB RESOURCES_FILES ${RESOURCES_DIR}/*)

find_package(Qt5 COMPONENTS Core Widgets WebEngineWidgets Svg REQUIRED)

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

install(TARGETS ${STATEMACHINEVIEWER} DESTINATION statemachineviewer)
install_qt5_dlls(DLL_FILES Qt5Core Qt5Gui Qt5Widgets
					OUTPUT_DIR statemachineviewer)
install_qt_plugins(PLUGINS platform imageformats OUTPUT_DIR statemachineviewer)
install(FILES $<TARGET_RUNTIME_DLLS:${STATEMACHINEVIEWER}> DESTINATION statemachineviewer)
