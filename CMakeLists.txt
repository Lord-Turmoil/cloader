cmake_minimum_required(VERSION 3.21)

# Display name used by the documentation (the CMake project itself is lowercase
# "cloader" so that find_package(cloader) looks natural).
set(CLOADER_PROJECT_NAME "Cloader")

include(cmake/version.cmake)

project(
    cloader
    VERSION "${CLOADER_VERSION}"
    DESCRIPTION "A tiny cross-platform C++ class loader library."
    HOMEPAGE_URL "https://github.com/Lord-Turmoil/cloader"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(cmake/common.cmake)

# ---------- Options ----------
# Everything beyond the library is opt-in, and the install rules are only on by
# default when cloader is the top-level project. Added with add_subdirectory()
# or FetchContent, cloader defines its targets and nothing else: no global
# settings, no output directories, no install rules, no CTest targets.
option(CLOADER_BUILD_TESTS      "Build the unit tests" OFF)
option(CLOADER_BUILD_EXAMPLES   "Build the example projects" OFF)
option(CLOADER_BUILD_DOCS       "Build the API documentation with Doxygen" OFF)
option(CLOADER_BUILD_COVERAGE   "Build with code coverage instrumentation" OFF)
option(CLOADER_INSTALL          "Generate the install rules and the CMake package" ${PROJECT_IS_TOP_LEVEL})

# Coverage data comes from running the unit tests, so coverage implies tests.
if(CLOADER_BUILD_COVERAGE AND NOT CLOADER_BUILD_TESTS)
    message(STATUS "CLOADER_BUILD_COVERAGE is ON, enabling CLOADER_BUILD_TESTS")
    set(CLOADER_BUILD_TESTS ON CACHE BOOL "Build the unit tests" FORCE)
endif()

# ---------- Top-level conveniences ----------
if(PROJECT_IS_TOP_LEVEL)
    # Binaries in bin/ and libraries in lib/, where the example scripts look for them.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

    # Keep local builds self-contained: "cmake --install" drops everything into
    # install/ next to the sources instead of /usr/local.
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(
            CMAKE_INSTALL_PREFIX
            "${PROJECT_SOURCE_DIR}/install"
            CACHE PATH "Install path" FORCE
        )
    endif()

    message(STATUS "Cloader version ${PROJECT_VERSION}")
    message(STATUS "Cloader build type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "Cloader build examples: ${CLOADER_BUILD_EXAMPLES}")
    message(STATUS "Cloader build docs: ${CLOADER_BUILD_DOCS}")
    message(STATUS "Cloader build tests: ${CLOADER_BUILD_TESTS}")
    message(STATUS "Cloader build coverage: ${CLOADER_BUILD_COVERAGE}")
    message(STATUS "Cloader install: ${CLOADER_INSTALL}")
else()
    message(STATUS "Cloader ${PROJECT_VERSION} (subproject)")
endif()

# ---------- Targets ----------
add_subdirectory(cloader)

if(CLOADER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

cloader_configure_coverage()

if(CLOADER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CLOADER_BUILD_DOCS)
    add_subdirectory(docs)
endif()

# ---------- Package ----------
# Lets other projects use an installed cloader with find_package(cloader).
if(CLOADER_INSTALL)
    include(CMakePackageConfigHelpers)

    set(cloader_config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/cloader")

    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/cloaderConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/cloaderConfig.cmake"
        INSTALL_DESTINATION "${cloader_config_install_dir}"
    )

    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/cloaderConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
    )

    install(
        EXPORT cloader-targets
        NAMESPACE cloader::
        DESTINATION "${cloader_config_install_dir}"
        COMPONENT cloader_Development
    )

    install(
        FILES
            "${PROJECT_BINARY_DIR}/cloaderConfig.cmake"
            "${PROJECT_BINARY_DIR}/cloaderConfigVersion.cmake"
        DESTINATION "${cloader_config_install_dir}"
        COMPONENT cloader_Development
    )
endif()
