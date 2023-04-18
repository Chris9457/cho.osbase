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

add_UT(
	UT ${APPLICATION}.${UNIT_TEST}
	SRC_DIR ${CMAKE_SOURCE_DIR}/src/osApplication
)

set(TESTDOMAINOBJECTS_API ${RESOURCES_DIR}/TestDomainObjects.yml)
set(TESTDOMAINOBJECTSIMPORT_API ${RESOURCES_DIR}/TestDomainObjectsImport.yml)
osbase_service_generate_stub(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTDOMAINOBJECTS_API} API_DEPENDS ${TESTDOMAINOBJECTSIMPORT_API})

set(TESTPROCESS_API ${RESOURCES_DIR}/TestProcess.yml)
osbase_service_generate_stub(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTPROCESS_API})
osbase_service_generate_impl_skeleton(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTPROCESS_API})

set(TESTEVENT_API ${RESOURCES_DIR}/TestEvent.yml)
osbase_service_generate_stub(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTEVENT_API})
osbase_service_generate_impl_skeleton(TARGET ${APPLICATION}.${UNIT_TEST} API ${TESTEVENT_API})

 