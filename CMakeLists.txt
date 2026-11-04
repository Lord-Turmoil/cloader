cmake_minimum_required(VERSION 3.21)

# Display name used by the documentation (the CMake project itself is lowercase
# "cloader" so that find_package(cloader) looks natural).
set(CLOADER_PROJECT_NAME "Cloader")

include(cmake/version.cmake)
include(cmake/common.cmake)

project(
    cloader
    VERSION "${CLOADER_VERSION}"
    DESCRIPTION "A tiny cross-platform C++ class loader library."
    HOMEPAGE_URL "https://github.com/Lord-Turmoil/cloader"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Only build tests when this project is built on its own, not when it is
# pulled in as a dependency.
option(BUILD_TESTING "Build tests" ${PROJECT_IS_TOP_LEVEL})
include(CTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(CLOADER_BUILD_EXAMPLES "Build the example projects" OFF)
option(CLOADER_BUILD_DOCS "Build the API documentation with Doxygen" OFF)

# Keep local builds self-contained: "cmake --install" drops everything into
# install/ next to the sources instead of /usr/local.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(
        CMAKE_INSTALL_PREFIX
        "${CMAKE_CURRENT_SOURCE_DIR}/install"
        CACHE PATH "Install path" FORCE
    )
endif()

add_subdirectory(cloader)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(CLOADER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CLOADER_BUILD_DOCS)
    add_subdirectory(docs)
endif()

set(cloader_config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/cloader")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cloaderConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cloaderConfig.cmake"
    INSTALL_DESTINATION "${cloader_config_install_dir}"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cloaderConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

install(
    EXPORT cloader-targets
    NAMESPACE cloader::
    DESTINATION "${cloader_config_install_dir}"
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cloaderConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cloaderConfigVersion.cmake"
    DESTINATION "${cloader_config_install_dir}"
)
