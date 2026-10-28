cmake_minimum_required(VERSION 3.21)

include(cmake/version.cmake)
include(cmake/common.cmake)

project(cloader VERSION "${CLOADER_VERSION}" LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(CLOADER_BUILD_EXAMPLES "Build examples" OFF)
if(PROJECT_IS_TOP_LEVEL)
    set(CLOADER_IS_TOP_LEVEL TRUE)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(
        CMAKE_INSTALL_PREFIX
        "${CMAKE_CURRENT_SOURCE_DIR}/install"
        CACHE PATH "Install path" FORCE
    )
endif()

add_subdirectory(cloader)

if(CLOADER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set(cloader_config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/cloader")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cloader-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cloader-config.cmake"
    INSTALL_DESTINATION "${cloader_config_install_dir}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cloader-config-version.cmake"
    COMPATIBILITY SameMajorVersion
)

install(
    EXPORT cloader-targets
    NAMESPACE cloader::
    DESTINATION "${cloader_config_install_dir}"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cloader-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cloader-config-version.cmake"
    DESTINATION "${cloader_config_install_dir}"
)
