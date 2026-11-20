# The cloader library. Two targets:
#
#   cloader::cloader  The static library, for applications that load plugins:
#                     ClassLoader, the exceptions and the logging.
#   cloader::headers  Headers only, for plugin libraries: Loadable, the factory
#                     macros and CLOADER_EXPORT. Linking it instead of
#                     cloader::cloader keeps a second copy of the loader's state
#                     out of every plugin. A plugin that throws cloader's
#                     exceptions or uses ClassLoader itself links cloader::cloader.

set(cloader_generated_include_dir "${CMAKE_CURRENT_BINARY_DIR}/include")

configure_file(
    include/cloader/version.h.in
    "${cloader_generated_include_dir}/cloader/version.h"
    @ONLY
)

# ---------- cloader::headers ----------
add_library(cloader_headers INTERFACE)
add_library(cloader::headers ALIAS cloader_headers)

target_include_directories(
    cloader_headers
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${cloader_generated_include_dir}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_compile_features(cloader_headers INTERFACE cxx_std_20)
set_target_properties(cloader_headers PROPERTIES EXPORT_NAME headers)

# ---------- cloader::cloader ----------
set(
    cloader_public_headers
    include/cloader/cloader.h
    include/cloader/ClassLoader.h
    include/cloader/Exception.h
    include/cloader/Loadable.h
    include/cloader/details/Config.h
    include/cloader/details/Logging.h
    "${cloader_generated_include_dir}/cloader/version.h"
)

set(
    cloader_sources
    src/ClassLoader.cpp
    src/Exception.cpp
    src/details/Logging.cpp
    src/details/NativeLoader.cpp
    src/details/NativeLoader.h
)
if(WIN32)
    list(APPEND cloader_sources src/details/NativeLoaderWindows.cpp)
else()
    list(APPEND cloader_sources src/details/NativeLoaderUnix.cpp)
endif()

# Always static: plugins must not carry their own copy of the loader's state, and
# the library has no import/export macros for a DLL build. It is position
# independent so that it can be linked into shared libraries.
add_library(cloader STATIC ${cloader_sources} ${cloader_public_headers})
add_library(cloader::cloader ALIAS cloader)

target_link_libraries(
    cloader
    PUBLIC
        cloader_headers
    PRIVATE
        # dlopen() lives in libdl on glibc before 2.34; empty on other platforms.
        ${CMAKE_DL_LIBS}
)
target_include_directories(cloader PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
cloader_configure_compiler(cloader)

set_target_properties(
    cloader
    PROPERTIES
        EXPORT_NAME cloader
        POSITION_INDEPENDENT_CODE ON
)

# ---------- Install ----------
if(CLOADER_INSTALL)
    install(
        DIRECTORY include/cloader
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT cloader_Development
        FILES_MATCHING
            PATTERN "*.h"
    )
    install(
        FILES "${cloader_generated_include_dir}/cloader/version.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cloader"
        COMPONENT cloader_Development
    )
    install(
        TARGETS cloader cloader_headers
        EXPORT cloader-targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT cloader_Development
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif()
