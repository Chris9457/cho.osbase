set(APPLICATION ${OSBASE}.application)

set(INCLUDE_DIR include)
set(SRC_DIR src)
set(RESOURCES_DIR resources)
file(GLOB_RECURSE INCLUDE_FILES ${INCLUDE_DIR}/*)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*)
file(GLOB RESOURCES_FILES ${RESOURCES_DIR}/*)

add_subdirectory(TestService)
add_subdirectory(TestServiceImpl)

add_executable(${APPLICATION}.${UNIT_TEST} ${INCLUDE_FILES} ${SRC_FILES} ${RESOURCES_FILES})
target_link_libraries(${APPLICATION}.${UNIT_TEST} PRIVATE 
					  ${APPLICATION} 
					  ${APPLICATION}.${UNIT_TEST}.TestService.api
					  ${APPLICATION}.${UNIT_TEST}.TestService.impl
					  ${OSBASE}.data.impl
					  ${OSBASE}.core.impl
					  )
target_include_directories(${APPLICATION}.${UNIT_TEST} PRIVATE ${INCLUDE_DIR})
target_compile_definitions(${APPLICATION}.${UNIT_TEST} PUBLIC TIMEOUT_FACTOR=1)

add_UT(UT ${APPLICATION}.${UNIT_TEST})

set(TESTDOMAINOBJECTS_API ${CMAKE_CURRENT_BINARY_DIR}/TestDomainObjects.yml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCES_DIR}/TestDomainObjects.yml.in ${TESTDOMAINOBJECTS_API})

set(TESTDOMAINOBJECTSIMPORT_API ${CMAKE_CURRENT_BINARY_DIR}/TestDomainObjectsImport.yml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCES_DIR}/TestDomainObjectsImport.yml.in ${TESTDOMAINOBJECTSIMPORT_API})

osbase_service_generate_stub(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTDOMAINOBJECTS_API} API_DEPENDS ${TESTDOMAINOBJECTSIMPORT_API})

set(TESTPROCESS_API ${CMAKE_CURRENT_BINARY_DIR}/TestProcess.yml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCES_DIR}/TestProcess.yml.in ${TESTPROCESS_API})

osbase_service_generate_stub(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTPROCESS_API})
osbase_service_generate_impl_skeleton(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTPROCESS_API})

set(TESTEVENT_API ${CMAKE_CURRENT_BINARY_DIR}/TestEvent.yml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCES_DIR}/TestEvent.yml.in ${TESTEVENT_API})

osbase_service_generate_stub(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTEVENT_API})
osbase_service_generate_impl_skeleton(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTEVENT_API})

 