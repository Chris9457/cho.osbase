include(cmake/utils.cmake)

set(UNIT_TEST ut)
set(BENCHMARK bm)

add_subdirectory(osCore_UT)
add_subdirectory(osCore_BM)
add_subdirectory(osCoreImpl_UT)
add_subdirectory(osData_UT)
add_subdirectory(osApplication_UT)
add_subdirectory(osApplication_BM)
add_subdirectory(osStateMachine_UT)
add_subdirectory(osWebEngine_UT)
add_subdirectory(osWebViewer_UT)
