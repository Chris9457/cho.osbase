if (VCPKG_TARGET_IS_WINDOWS)
    message("Shared build is broken under Windows. See https://github.com/darrenjs/wampcc/issues/57")
    vcpkg_check_linkage(ONLY_STATIC_LIBRARY)
endif()


vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO darrenjs/wampcc
    REF 73a4aee7a83328409ab59d6b1bb9d9ae382bbb95
    SHA512 bdf2278a14c46107b5d8089132f79a6c248aaee9488a9417c0d2db98d62734fa9e23cd3564e1c4b75b6bad2597997a20867cb1ad91f56914ceda8b22bb0883b7
    HEAD_REF master
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        examples    BUILD_EXAMPLES
        utils       BUILD_UTILS
)


vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_TESTS:BOOL=OFF # Tests build is broken
        ${FEATURE_OPTIONS}
)
vcpkg_cmake_install()

file(COPY "${CURRENT_PACKAGES_DIR}/debug/lib/cmake/wampcc/wampccTargets-debug.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/lib/cmake/wampcc")
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/wampcc")

if("utils" IN_LIST FEATURES)
    vcpkg_copy_tools(TOOL_NAMES admin AUTO_CLEAN)
endif()

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_fixup_pkgconfig()

file(READ "${CURRENT_PACKAGES_DIR}/share/${PORT}/wampccTargets.cmake" _contents)
string(REPLACE "LibUV::LibUV" "uv" _contents "${_contents}")
string(REPLACE "jansson" "jansson::jansson" _contents "${_contents}")
file(WRITE "${CURRENT_PACKAGES_DIR}/share/${PORT}/wampccTargets.cmake" "${_contents}")
file(APPEND "${CURRENT_PACKAGES_DIR}/share/${PORT}/wampccTargets.cmake" "\nfind_package(OpenSSL REQUIRED)")
file(APPEND "${CURRENT_PACKAGES_DIR}/share/${PORT}/wampccTargets.cmake" "\nfind_package(LibUV CONFIG REQUIRED)")
file(APPEND "${CURRENT_PACKAGES_DIR}/share/${PORT}/wampccTargets.cmake" "\nfind_package(Jansson CONFIG REQUIRED)")
