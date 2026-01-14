# Package configuration for calc

# Generate config files for downstream consumers
include(CMakePackageConfigHelpers)

# Config version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/calc-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/calc-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/calc-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/calc
)

# Install config files
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/calc-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/calc-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/calc
)

# Export targets
install(
    EXPORT calcTargets
    FILE calc-targets.cmake
    NAMESPACE calc::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/calc
)
